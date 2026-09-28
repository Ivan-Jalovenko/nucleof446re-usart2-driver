#include "ring_buffer.h"

void ring_buffer_init(RingBuffer_t* ring_buffer) {
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
}

RingBuffer_State_e ring_buffer_push(RingBuffer_t* ring_buffer, uint8_t data) {
    if (ring_buffer_is_full(ring_buffer)) {
        return RingBuffer_Full;
    }   
    
    uint8_t new_head = (ring_buffer->head + 1) % RING_BUFFER_SIZE;
    ring_buffer->buffer[ring_buffer->head] = data;
    ring_buffer->head = new_head;

    return RingBuffer_Ok;
}

RingBuffer_State_e ring_buffer_pop(RingBuffer_t* ring_buffer, uint8_t* data) {
    if (ring_buffer_is_empty(ring_buffer)) {
        return RingBuffer_Empty;
    }
    if (data == NULL) {
        return RingBuffer_InvalidArg;
    }

    *data = ring_buffer->buffer[ring_buffer->tail];
    ring_buffer->tail = (ring_buffer->tail + 1) % RING_BUFFER_SIZE;

    return RingBuffer_Ok;
}

bool ring_buffer_is_empty(RingBuffer_t* ring_buffer) {
    if (ring_buffer->head == ring_buffer->tail) {
        return true;
    }
    return false;
}

bool ring_buffer_is_full(RingBuffer_t* ring_buffer) {
    if ((ring_buffer->head + 1) % RING_BUFFER_SIZE == ring_buffer->tail) {
        return true;
    }
    return false;
}
