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
