/*
This is a simulation of a cooling system. 

Two devices (an ECU and a controller ) are talking over a CAN bus 

At the start, a few settings are read from the command line 

During each time step of the simulation:

1- The ECU will send its current temperature and ignition status.

2- The controller reads those values, runs a PID control loop and a state machine to figure out how much to turn on the pump and fan.

e- It sends those commands back to the ECU.

4- The ECU applies them, and the plant’s temperature changes accordingly.

The program keeps printing the system’s status — time, temperature, and actuator levels — until the simulation finishes.

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "../include/can_bus.h"
#include "../include/pid.h"
#include "../include/fsm.h"
#include "../include/plant.h"
#include "../include/cli.h"

/*  CAN IDs  */
#define ID_TEMP_REPORT  0x100U
#define ID_IGN_STATUS   0x101U
#define ID_PUMP_CMD     0x200U
#define ID_FAN_CMD      0x201U


static const char* state_str(FsmState s){
    switch (s) {
        case ST_OFF:     return "OFF";
        case ST_IDLE:    return "IDLE";
        case ST_COOLING: return "COOLING";
        case ST_FAULT:   return "FAULT";
        default:         return "?";
    }
}

int main(int argc, char** argv)
{
    /*  CLI params  */
    Params par;
    params_defaults(&par);
    if (!params_parse(&par, argc, argv)) return 0;
    params_print(&par);

    /* Plant + control  */
    Plant plant; 
    plant_init(&plant, par.ambient_c, par.ambient_c + 20.0f);

    PID pid;    
    pid_init(&pid, par.kp, par.ki, par.kd, 0, 100);
    
    Fsm fsm;    
    fsm_init(&fsm);

    Actuators act = (Actuators){0};

    /*  Simulating CAN bus with two nodes  */
    CANBus bus;   bus_init(&bus);
    CANNode ctrl; node_init(&ctrl, &bus);
    CANNode ecu;  node_init(&ecu,  &bus);

    /*  Sim loop timing  */
    const float dt = 0.5f;
    const int   steps = (int)(par.duration_s / dt);
    int ignition_on = par.ignition_on;

    for (int k = 0; k < steps; ++k) {
        /*  ECU publishes telemetry (TEMP, IGN)  */
        {
            struct can_frame ft = {0};
            ft.can_id  = ID_TEMP_REPORT;
            ft.can_dlc = 4;
            memcpy(ft.data, &plant.temp_c, 4);
            node_send(&ecu, ft.can_id, ft.data, ft.can_dlc);

            struct can_frame fi = {0};
            fi.can_id  = ID_IGN_STATUS;
            fi.can_dlc = 1;
            fi.data[0] = (uint8_t)(ignition_on ? 1 : 0);
            node_send(&ecu, fi.can_id, fi.data, fi.can_dlc);
        }

        /* Move telemetry from bus → Controller’s inbox */
        while (node_pull_from_bus(&ctrl)) { /* drain all */ }

        /*  Controller use telemetry to computes commands */
        float temp_seen = plant.temp_c;
        int   ign_seen  = ignition_on;

        struct can_frame rx;
        while (node_read(&ctrl, &rx)) {
            if (rx.can_id == ID_TEMP_REPORT && rx.can_dlc >= 4) {
                memcpy(&temp_seen, rx.data, 4);
            } else if (rx.can_id == ID_IGN_STATUS && rx.can_dlc >= 1) {
                ign_seen = (rx.data[0] != 0);
            }
        }

        /* PID + FSM on Controller side */
        float err       = par.setpoint_c - temp_seen;
        float pid_out   = pid_update(&pid, err, dt);
        fsm_step(&fsm, ign_seen, temp_seen, par.setpoint_c,
                 pid_out, par.fan_on_delta_c, &act);

        /* Controller sends PUMP + FAN commands to ECU */
        {
            struct can_frame fp = {0};
            fp.can_id  = ID_PUMP_CMD;
            fp.can_dlc = 4;
            memcpy(fp.data, &act.pump_duty, 4);
            node_send(&ctrl, fp.can_id, fp.data, fp.can_dlc);

            struct can_frame ff = {0};
            ff.can_id  = ID_FAN_CMD;
            ff.can_dlc = 4;
            memcpy(ff.data, &act.fan_duty, 4);
            node_send(&ctrl, ff.can_id, ff.data, ff.can_dlc);
        }

        /* Move commands from bus → ECU inbox */
        while (node_pull_from_bus(&ecu)) { /* drain all */ }

        /*  ECU applies received commands to the plant  */
        while (node_read(&ecu, &rx)) {
            if (rx.can_id == ID_PUMP_CMD && rx.can_dlc >= 4) {
                memcpy(&act.pump_duty, rx.data, 4);
            } else if (rx.can_id == ID_FAN_CMD && rx.can_dlc >= 4) {
                memcpy(&act.fan_duty, rx.data, 4);
            }
        }

        /* Plant step */
        plant_step(&plant, act.pump_duty, act.fan_duty, dt);

        /* Status print */
        printf("t=%4.1fs  ST=%s  Temp=%.2fC  Set=%.2fC  Pump=%5.1f%%  Fan=%5.1f%%\n",
               (k + 1) * dt, state_str(fsm.state),
               temp_seen, par.setpoint_c,
               act.pump_duty, act.fan_duty);

        usleep((useconds_t)(dt * 1000000.0f));
    }

    return 0;
}
