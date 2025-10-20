#include <gtest/gtest.h>
extern "C" {
#include "../include/pid.h"
}

/*
 * Simple PID test: the temperature should move toward the setpoint.
 */
TEST(PID, BasicConverges) {
  PID p; 
  pid_init(&p, 2.0f, 0.5f, 0.1f, 0.f, 100.f);  // setup PID gains

  float temp = 60.f, set = 40.f;
  for (int i = 0; i < 200; i++) {
    float u = pid_update(&p, set - temp, 0.1f);  // compute control output
    temp += -0.02f * u;                          // apply control effect
  }

  EXPECT_LT(temp, 42.f);  // expect system to approach setpoint
}
