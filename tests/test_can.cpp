#include <gtest/gtest.h>
extern "C" {
#include "../include/can_bus.h"
}

/* -------------------------------------------------------
 * Test 1: send/receive between two nodes
 * -----------------------------------------------------*/
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

/* -------------------------------------------------------
 * Test 2: ID/MASK filters for two nodes
*/

TEST(CAN, FiltersAcceptCorrectIDs) {
    CANBus bus;
    bus_init(&bus);

    CANNode ctrl, ecu;
    node_init(&ctrl, &bus);
    node_init(&ecu, &bus);

    // Each node accepts two message IDs
    node_set_filter(&ctrl, 0x100U, 0x7FEU);  // accepts 0x100 & 0x101
    node_filter_enable(&ctrl, 1);

    node_set_filter(&ecu,  0x200U, 0x7FEU);  // accepts 0x200 & 0x201
    node_filter_enable(&ecu, 1);

    // Send 6 frames: 4 valid, 2 noise
    uint8_t data[4] = {1, 2, 3, 4};

    // For controller
    node_send(&ctrl, 0x100U, data, 4);
    node_send(&ctrl, 0x101U, data, 4);

    // For ECU
    node_send(&ctrl, 0x200U, data, 4);
    node_send(&ctrl, 0x201U, data, 4);

    // Noise (should be ignored)
    node_send(&ctrl, 0x123U, data, 4);
    node_send(&ctrl, 0x2FFU, data, 4);

    // both nodes fetch their frames from the bus
    for (int i = 0; i < 100; ++i) {
        if (!(node_pull_from_bus(&ctrl) || node_pull_from_bus(&ecu)))
            break;
    }

    // Check controller inbox 
    int ctrl_count = 0;
    struct can_frame f;
    while (node_read(&ctrl, &f)) {
        ++ctrl_count;
        EXPECT_TRUE(f.can_id == 0x100U || f.can_id == 0x101U)
            << std::hex << "Wrong ID in ctrl inbox: 0x" << f.can_id;
    }
    EXPECT_EQ(ctrl_count, 2);

    // Check ECU inbox 
    int ecu_count = 0;
    while (node_read(&ecu, &f)) {
        ++ecu_count;
        EXPECT_TRUE(f.can_id == 0x200U || f.can_id == 0x201U)
            << std::hex << "Wrong ID in ecu inbox: 0x" << f.can_id;
    }
    EXPECT_EQ(ecu_count, 2);
}
