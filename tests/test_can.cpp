#include <gtest/gtest.h>
extern "C" {
#include "../include/can_bus.h"
}

/*
 * This test checks basic CAN bus communication between two nodes.
 * One node sends a frame, and the other should receive it correctly.
 */
TEST(CAN, SendReceiveOneFrame) {
  CANBus b; 
  bus_init(&b);          // initialize the bus

  CANNode a, bn;
  node_init(&a, &b);     // controller node
  node_init(&bn, &b);    // ECU node

  // Prepare a frame with two bytes of data
  uint8_t d[2] = {1, 2};
  ASSERT_TRUE(node_send(&a, 0x123, d, 2));   // send frame from node A
  ASSERT_TRUE(node_pull_from_bus(&bn));      // move frame from bus to node B inbox

  struct can_frame rx;
  ASSERT_TRUE(node_read(&bn, &rx));          // node B reads received frame

  // Verify that frame contents are correct
  EXPECT_EQ(rx.can_id, 0x123u);
  EXPECT_EQ(rx.can_dlc, 2);
  EXPECT_EQ(rx.data[0], 1);
  EXPECT_EQ(rx.data[1], 2);
}
