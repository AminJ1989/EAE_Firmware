#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* PID control structure */
typedef struct {
    float kp, ki, kd;        // gains
    float prev_err;          // previous error
    float integ;             // integral term
    float out_min, out_max;  // output limits
} PID;

/* setup and update */
void  pid_init(PID* p, float kp, float ki, float kd, float out_min, float out_max);
float pid_update(PID* p, float err, float dt);

#ifdef __cplusplus
}
#endif

