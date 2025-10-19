#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/* runtime configuration */
typedef struct {
    float ambient_c;      /* ambient temperature */
    float setpoint_c;     /* target temperature */
    float kp, ki, kd;     /* PID gains */
    float duration_s;     /* simulation time */
    int   ignition_on;    /* 1=on, 0=off */
    float fan_on_delta_c; /* delta above setpoint to start fan */
} Params;

/* default values, CLI parse, and print helpers */
void params_defaults(Params* p);
int  params_parse(Params* p, int argc, char** argv);
void params_print(const Params* p);

#ifdef __cplusplus
}
#endif

