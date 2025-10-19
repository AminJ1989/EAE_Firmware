#include "../include/pid.h"

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
    
    float e = -err;

    p->integ += e * dt;
    float deriv = (dt > 0.0f) ? (e - p->prev_err) / dt : 0.0f;
    float u = p->kp*e + p->ki*p->integ + p->kd*deriv;
    p->prev_err = e;

    return clampf(u, p->out_min, p->out_max);
}
