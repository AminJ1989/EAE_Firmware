#pragma once
#include <stdint.h>

#ifndef CAN_QUEUE_CAP
#define CAN_QUEUE_CAP 64
#endif

/* CAN frame */
struct can_frame {
    uint32_t can_id;
    uint8_t  can_dlc;
    uint8_t  data[8];
};

/* ring buffer */
typedef struct {
    struct can_frame buf[CAN_QUEUE_CAP];
    int head, tail, count;
} CANQueue;

/* use the queue as the bus */
typedef CANQueue CANBus;

/* one node on the bus */
typedef struct {
    CANBus*  bus;
    CANQueue inbox;
} CANNode;


