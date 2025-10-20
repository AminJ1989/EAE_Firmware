#include "../include/can_bus.h"
#include <string.h>



static int id_accepts(const CANNode* n, uint32_t frame_can_id){
    if (!n->filter_on)      return 1;      /* filter disabled -> accept all */
    if (n->filter_mask==0U) return 1;      /* mask=0 -> accept all */

    uint32_t fid = (frame_can_id & CAN_SFF_MASK) ;
    uint32_t rid =(n->filter_id & CAN_SFF_MASK) ;
    uint32_t msk = n->filter_mask;

    return ((fid & msk) == (rid & msk));
}

/* set ID and mask for filter */
void node_set_filter(CANNode* n, uint32_t id, uint32_t mask) {
    if (!n) return;
    n->filter_id   = id;
    n->filter_mask = mask;
}

/* enable or disable filter */
void node_filter_enable(CANNode* n, int enable) {
    if (!n) return;
    n->filter_on = (enable != 0);
}


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
    n->filter_id   = 0U;
    n->filter_mask = 0U;
    n->filter_on   = 0U;  /* disabled by default */

}


/* send frame (writing to CAN TX mailbox) */
int node_send(CANNode* from, uint32_t id, const uint8_t* data, uint8_t dlc){
    if (!from || !from->bus) return 0;
    struct can_frame f = {0};
    f.can_id  = id;
    f.can_dlc = (dlc > 8) ? 8 : dlc;
    if (data && f.can_dlc) memcpy(f.data, data, f.can_dlc);
    return q_push(from->bus, &f);
}

/* receive matching frame from bus to node inbox*/
int node_pull_from_bus(CANNode* n){
    if (!n || !n->bus) return 0;
    if (n->inbox.count >= CAN_QUEUE_CAP) return 0;

    CANQueue* bus = n->bus;
    if (bus->count <= 0) return 0;

    struct can_frame f;
    int scanned = 0;
    int bus_size = bus->count;  /* snapshot to avoid endless loop as we requeue */

    while (scanned < bus_size && q_pop(bus, &f)) {
        scanned++;

        if (id_accepts(n, f.can_id)) {
            /* getting matching frame*/
            return q_push(&n->inbox, &f);
        } else {
            /* put back for other nodes to see */
            q_push(bus, &f);
        }
    }
    return 0;
}

/* read received frame */
int node_read(CANNode* n, struct can_frame* out){
    if (!n || !out) return 0;
    return q_pop(&n->inbox, out);
}
