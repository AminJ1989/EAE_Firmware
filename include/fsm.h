#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/* outputs */
typedef struct {
    float pump_duty; /* 0..100 (%) */
    float fan_duty;  /* 0..100 (%) */
} Actuators;

/* system states */
typedef enum {
    ST_OFF = 0,
    ST_IDLE,
    ST_COOLING,
    ST_FAULT
} FsmState;

/* FSM control block */
typedef struct {
    FsmState state;
    int bad_sensor; /* fault flag */
} Fsm;

/* init and step */
void fsm_init(Fsm* f);
void fsm_step(Fsm* f, int ignition_on, float temp_c, float setpoint_c,
              float pid_out, float fan_on_delta_c,
              Actuators* act);

#ifdef __cplusplus
}
#endif

