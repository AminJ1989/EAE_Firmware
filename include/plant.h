#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/* simple thermal model */
typedef struct {
    float ambient_c; /* ambient temperature */
    float temp_c;    /* current temperature */
} Plant;

/* init and update */
void plant_init(Plant* p, float ambient_c, float start_temp_c);
void plant_step(Plant* p, float pump_duty, float fan_duty, float dt_s);

#ifdef __cplusplus
}
#endif

