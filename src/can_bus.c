#include "../include/can_bus.h"
#include <string.h>

/* ring buffer ( CAN TX/RX FIFO) */
static void q_init(CANQueue* q){ q->head=q->tail=q->count=0; }

static int  q_push(CANQueue* q, const struct can_frame* f){
    if (q->count >= CAN_QUEUE_CAP) return 0;          // TX buffer full
    q->buf[q->tail] = *f;
    q->tail = (q->tail + 1) % CAN_QUEUE_CAP;
    q->count++;
    return 1;
}
static int  q_pop(CANQueue* q, struct can_frame* out){
    if (q->count <= 0) return 0;                      // RX buffer empty
    *out = q->buf[q->head];
    q->head = (q->head + 1) % CAN_QUEUE_CAP;
    q->count--;
    return 1;
}

/* init bus (enabling CAN peripheral) */
void bus_init(CANBus* b){
    if (!b) return;
    q_init(b);
}

/* init node (setting CAN filters and linking to bus) */
void node_init(CANNode* n, CANBus* b){
    if (!n) return;
    n->bus = b;
    q_init(&n->inbox);
}

/* transmit frame (writing to CAN TX mailbox) */
int node_send(CANNode* from, uint32_t id, const uint8_t* data, uint8_t dlc){
    if (!from || !from->bus) return 0;
    struct can_frame f = {0};
    f.can_id  = id;
    f.can_dlc = (dlc > 8) ? 8 : dlc;
    if (data && f.can_dlc) memcpy(f.data, data, f.can_dlc);
    return q_push(from->bus, &f);
}

/* receive frame (moving RX FIFO message to node buffer) */
int node_pull_from_bus(CANNode* n){
    if (!n || !n->bus) return 0;
    if (n->inbox.count >= CAN_QUEUE_CAP) return 0;
    struct can_frame f;
    if (!q_pop(n->bus, &f)) return 0;
    return q_push(&n->inbox, &f);
}

/* read received frame */
int node_read(CANNode* n, struct can_frame* out){
    if (!n || !out) return 0;
    return q_pop(&n->inbox, out);
}
