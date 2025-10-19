#include "../include/plant.h"

/* init model */
void plant_init(Plant* p, float ambient_c, float start_temp_c){
    p->ambient_c = ambient_c;
    p->temp_c    = start_temp_c;
}

/* simple thermal response */
void plant_step(Plant* p, float pump_duty, float fan_duty, float dt_s){
    float activity = (pump_duty + fan_duty) * 0.01f; /* 0..2 */
    float change;

    if (activity > 0.f) {
        /* cooling effect */
        change = - (0.15f * activity) - (0.02f * activity * activity);
    } else {
        /* drift toward ambient */
        change = (p->ambient_c - p->temp_c) * 0.02f;
    }

    p->temp_c += change * dt_s * 10.0f; /* scale for faster sim response */
}

