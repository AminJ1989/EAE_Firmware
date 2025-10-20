
#include "../include/fsm.h"


void fsm_init(Fsm* f){
    f->state = ST_OFF;
    f->bad_sensor = 0;
}

void fsm_step(Fsm* f, int ignition_on, float temp_c, float setpoint_c,
              float pid_out, float fan_on_delta_c,
              Actuators* act)
{
    /* OFF state if ignition is off */
    if (!ignition_on) {
        f->state = ST_OFF;
    }

    switch (f->state) {

    case ST_OFF:
        /* outputs */
        act->pump_duty = 0.f;
        act->fan_duty  = 0.f;

        /* transition */
        if (ignition_on) {
            f->state = ST_IDLE;
        }
        break;

    case ST_IDLE:
        /* outputs */
        act->pump_duty = 10.f;  /* small circulation */
        act->fan_duty  = 0.f;

        /* transition */
        if (temp_c > setpoint_c + fan_on_delta_c) {
            f->state = ST_COOLING;
        }
        break;

    case ST_COOLING: {
        /* clamp pid_out between 0 and 1 */
        float pid_val = pid_out;
        if (pid_val < 0.f) pid_val = 0.f;
        if (pid_val > 1.f) pid_val = 1.f;

        /* outputs */
        act->pump_duty = pid_val;
        act->fan_duty  = pid_val * 0.8f;

        /* transition */
        if (temp_c <= setpoint_c) {
            f->state = ST_IDLE;
        }
        break;
    }

    default:
        f->state = ST_OFF;
        act->pump_duty = 0.f;
        act->fan_duty  = 0.f;
        break;
    }
}

