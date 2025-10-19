#include "../include/cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* set default runtime values */
void params_defaults(Params* p){
    p->ambient_c      = 25.0f;
    p->setpoint_c     = 40.0f;
    p->kp = 2.0f; p->ki = 0.5f; p->kd = 0.2f;
    p->duration_s     = 20.0f;
    p->ignition_on    = 1;
    p->fan_on_delta_c = 2.0f;
}

/* command-line parser: --key value */
int params_parse(Params* p, int argc, char** argv){
    for (int i = 1; i < argc; i++) {
        if (i + 1 >= argc) break;

        if      (strcmp(argv[i], "--ambient") == 0)      p->ambient_c = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--setpoint") == 0)     p->setpoint_c = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--kp") == 0)           p->kp = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--ki") == 0)           p->ki = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--kd") == 0)           p->kd = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--duration") == 0)     p->duration_s = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--ignition") == 0)     p->ignition_on = atoi(argv[++i]);
        else if (strcmp(argv[i], "--fan_delta") == 0)    p->fan_on_delta_c = (float)atof(argv[++i]);
        else if (strcmp(argv[i], "--help") == 0) {
            puts("Args: --ambient F  --setpoint F  --kp F --ki F --kd F  "
                 "--duration F  --ignition 0|1  --fan_delta F");
            return 0;
        }
    }
    return 1;
}

/* print current configuration */
void params_print(const Params* p){
    printf("[Params] ambient=%.1fC setpoint=%.1fC kp=%.2f ki=%.2f kd=%.2f "
           "duration=%.1fs ignition=%d fan_delta=%.1fC\n",
           p->ambient_c, p->setpoint_c, p->kp, p->ki, p->kd,
           p->duration_s, p->ignition_on, p->fan_on_delta_c);
}

