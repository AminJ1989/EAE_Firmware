
#include "../include/fsm.h"

static float clamp01(float x){ return x < 0.f ? 0.f : (x > 100.f ? 100.f : x); }

void fsm_init(Fsm* f){
    f->state = ST_OFF;
    f->bad_sensor = 0;
}

void fsm_step(Fsm* f, int ignition_on, float temp_c, float setpoint_c,
              float pid_out, float fan_on_delta_c, int sensor_ok,
              Actuators* act)
{
    act->pump_duty = 0.f;
    act->fan_duty  = 0.f;

    if (!sensor_ok) {
        f->bad_sensor = 1;
    }

    if (!ignition_on || f->bad_sensor) {
        f->state = ST_OFF;
        return;
    }

    switch (f->state) {
        case ST_OFF:
            f->state = ST_IDLE;
            break;
        case ST_IDLE:
            if (temp_c > setpoint_c + fan_on_delta_c) f->state = ST_COOLING;
            break;
        case ST_COOLING:
            if (temp_c <= setpoint_c) f->state = ST_IDLE;
            break;
        default: f->state = ST_OFF; break;
    }

    if (f->state == ST_IDLE) {
        act->pump_duty = 10.f; /* small circulation */
        act->fan_duty  = 0.f;
    } else if (f->state == ST_COOLING) {
        act->pump_duty = clamp01(pid_out);
        act->fan_duty  = clamp01(pid_out * 0.8f);
    }
}

