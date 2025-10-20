#include <gtest/gtest.h>
extern "C" {
#include "../include/pid.h"
}
TEST(PID, BasicConverges) {
  PID p; pid_init(&p, 2.0f, 0.5f, 0.1f, 0.f, 100.f);
  float temp = 60.f, set = 40.f;
  for (int i=0;i<200;i++) {
    float u = pid_update(&p, set - temp, 0.1f);
    temp += -0.02f * u;
  }
  EXPECT_LT(temp, 42.f);
}
