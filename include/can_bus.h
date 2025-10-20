#pragma once
#include <stdint.h>

#ifndef CAN_QUEUE_CAP
#define CAN_QUEUE_CAP 64
#endif


#ifndef CAN_EFF_FLAG
#define CAN_EFF_FLAG 0x80000000U
#endif
#ifndef CAN_EFF_MASK
#define CAN_EFF_MASK 0x1FFFFFFFU   /* 29-bit extended */
#endif
#ifndef CAN_SFF_MASK
#define CAN_SFF_MASK 0x7FFU        /* 11-bit standard */
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
    CANBus* bus;     // (wire)
    CANQueue inbox;  // local RX FIFO for received frames

    /* filter registers*/
    uint32_t filter_id;    /* filter ID register */
    uint32_t filter_mask;  /* filter mask register */
    uint8_t  filter_on;    /* 0: accept all, 1: apply (id&mask)==(filter&mask) */
} CANNode;

/* init */
void bus_init(CANBus* b);
void node_init(CANNode* n, CANBus* b);

/* io */
int  node_send(CANNode* from, uint32_t id, const uint8_t* data, uint8_t dlc);
int  node_pull_from_bus(CANNode* n);
int  node_read(CANNode* n, struct can_frame* out);


/* filter configuration */
void node_set_filter(CANNode* n, uint32_t id, uint32_t mask);
void node_filter_enable(CANNode* n, int enable);
