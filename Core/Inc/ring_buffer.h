#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define RING_BUFFER_SIZE 32

typedef enum {
    RingBuffer_Ok = 0,
    RingBuffer_Full,
    RingBuffer_Empty,
    RingBuffer_InvalidArg,
} RingBuffer_State_e;

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} RingBuffer_t;

void ring_buffer_init(RingBuffer_t* ring_buffer);

RingBuffer_State_e ring_buffer_push(RingBuffer_t* ring_buffer, uint8_t data);
RingBuffer_State_e ring_buffer_pop(RingBuffer_t* ring_buffer, uint8_t* data);

bool ring_buffer_is_empty(RingBuffer_t* ring_buffer);
bool ring_buffer_is_full(RingBuffer_t* ring_buffer);


#endif // RING_BUFFER_H
