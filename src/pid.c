#include "pid.h"

/* clamp value */
static float clampf(float v, float a, float b){ 
    return (v < a) ? a : (v > b ? b : v); 
}

/* init */
void pid_init(PID* p, float kp, float ki, float kd, float out_min, float out_max){
    p->kp = kp; p->ki = ki; p->kd = kd;
    p->prev_err = 0.0f;
    p->integ = 0.0f;
    p->out_min = out_min; 
    p->out_max = out_max;
}

/* update */
float pid_update(PID* p, float err, float dt){
    p->integ += err * dt;
    float deriv = (dt > 0.0f) ? (err - p->prev_err) / dt : 0.0f;
    float u = p->kp*err + p->ki*p->integ + p->kd*deriv;
    p->prev_err = err;
    return clampf(u, p->out_min, p->out_max);
}

