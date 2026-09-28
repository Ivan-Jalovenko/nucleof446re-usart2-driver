#ifndef USART2_DRIVER_H
#define USART2_DRIVER_H

#include <stdint.h>

#include "ring_buffer.h"


typedef enum {
    UART_Oversampling_8 = 0,
    UART_Oversampling_16,
} UART_Oversampling_e;

typedef enum {
    UART_StopBits_1 = 0,
    UART_StopBits_2,
} UART_StopBits_e;

typedef enum {
    RCC_InvalidBits = 0,
} RCC_Error_e;

typedef struct {
    uint32_t baud_rate;
    
    UART_Oversampling_e oversampling;
    UART_StopBits_e stop_bits;
    uint8_t interrupt_priority;
    
    uint8_t tx_enabled;
    uint8_t rx_enabled;
} UART_Config_t;


void USART2_ClockEnable();
void USART2_ClockDisable();

void USART2_GPIOClockEnable();
void USART2_GPIOClockDisable();

void USART2_Reset();

void USART2_Init(UART_Config_t config);

RingBuffer_State_e USART2_WriteChar(uint8_t data);
RingBuffer_State_e USART2_ReadChar(uint8_t* data);

#endif // UART_DRIVER_H
