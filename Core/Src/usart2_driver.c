#include "stm32f446xx.h"
#include "stm32f4xx.h"

#include "system_stm32f4xx.h"

#include "usart2_driver.h"
#include "ring_buffer.h"
#include "rcc_driver.h"
/**
 * TODO: Global
 * 1) Build reset function for USART2 - yes
 * 2) Write boilder plate functions to hide ring buffers with overhead - yes
 * 3) Determine how can i change word length since i do not have 9 bit variables - just hard code current uart for 8 bits 
 * 4) Set NVIC priority and determine which priority i do need for this - yes
 * 5) Fix the baud rate math 
 * 6) Get the information about current clock speed instead of just using define
 */

static RingBuffer_t tx_ring_buffer;
static RingBuffer_t rx_ring_buffer;

void USART2_ClockEnable() {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

void USART2_ClockDisable() {
    RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

void USART2_GPIOClockEnable() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
}

void USART2_GPIOClockDisable() {
    RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN;
}

void USART2_Reset() {
    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
}

uint32_t USART2_GetClockSpeed() {
    SystemCoreClockupdate();
    uint32_t clock_speed = SystemCoreClock;

    RCC_Presc_e AHB_presc = RCC_AHBPresc();
    RCC_Presc_e APB1_presc = RCC_APB1Presc();

    if (AHP_presc == RCC_Invalid) { return 0; }
    if (APB1_presc == RCC_Invalid) { return 0; }

    return clock_speed / AHB_presc / APB1_presc;
}


void USART2_Init(UART_Config_t config) {
    USART2_ClockEnable();
    USART2_GPIOClockEnable();
    USART2_Reset();

    ring_buffer_init(&tx_ring_buffer);
    ring_buffer_init(&rx_ring_buffer);

    /* Clear preivously set alternate funcitons from PA2 and PA3 */
    GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
    GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2_Msk;
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3_Msk;
    
    /* Set alternate functions for PA2(USART2_TX) and PA3(USART2_RX)*/
    GPIOA->MODER |= GPIO_MODER_MODE2_1; 
    GPIOA->MODER |= GPIO_MODER_MODE3_1;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2; // Corresponds to AF7 
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2; // Corresponds to AF7

    /* Set word length to 1 Start, 8 Data and 1 stop bit */
    USART2->CR1 &= ~USART_CR1_M;
    USART2->CR2 &= ~USART_CR2_STOP_Msk;

    /* Select oversampling by configuration */
    uint32_t oversampling = 0;
    uint8_t fraction_mask = 0x7;
    if (config.oversampling == UART_Oversampling_8) {
        oversampling = 8;
        USART2->CR1 |= USART_CR1_OVER8;
    } else {
        oversampling = 16;
        fraction_mask = 0xF;
        USART2->CR1 &= ~ USART_CR1_OVER8; // Oversampling by 16
    }


    /* Calculate load value of Baud Rate Register (BRR) */
    /*
                          F(ck)
    baud_rate = ----------------------------
                  oversampling * USART_DIV

    F(ck) = baud_rate * (oversampling * UASRT_DIV)
    
       F(ck)
    ----------- = oversampling * USART_DIV
     baud_rate

       F(ck)
    ---------- / oversampling = USART_DIV
     baud_rate


    mantissa is someting we can have by just derive, but how we can just optimize fraction division
    */
    uint32_t USART2_clock_speed = USART2_GetClockSpeed();
    double_t usart_div = (double_t)USART2_clock_speed / (config.baud_rate * oversampling);
    uint32_t mantissa = (uint32_t)usart_div;
    uint32_t fraction = (uint32_t)((usart_div - mantissa) * oversampling + 0.5);

    if (fraction >= oversampling) {
        mantissa += 1;
        fraction = 0;
    }

    USART2->BRR = (mantissa << USART_BRR_DIV_Mantissa_Pos) | (fraction & fraciton_mask);

    /* Enable interrupt flags */
    USART2->CR1 |= USART_CR1_TXEIE | USART_CR1_RXNEIE;

    /* Enable global interrupt for USART2 */
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 5);
    __enable_irq();

    /* Enable receiver and transmitter */
    USART2->CR1 |= USART_CR1_RE | USART_CR1_TE;

    USART2->CR1 |= USART_CR1_UE;
}

void USART2_IRQHandler() {
    uint32_t sr = USART2->SR;
    if (sr & USART_SR_RXNE) {
        uint8_t data = USART2->DR;
        if (!ring_buffer_is_full(&rx_ring_buffer)) {
            ring_buffer_push(&rx_ring_buffer, data);
        } else {
            (void)data;
        }
    }
    if (sr & USART_SR_TXE) {
        if (!ring_buffer_is_empty(&tx_ring_buffer)) {
            uint8_t data;
            ring_buffer_pop(&tx_ring_buffer, &data);
            USART2->DR = data;          
        } else {
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
    if (sr & USART_SR_ORE) {
        uint8_t data = USART2->DR;
        (void)data;
    }
}

RingBuffer_State_e USART2_WriteChar(uint8_t data) {
    if (ring_buffer_is_full(&tx_ring_buffer)) {
        return RingBuffer_Full;
    }
    
    ring_buffer_push(&tx_ring_buffer, data);
    USART2->CR1 |= USART_CR1_TXEIE;
    return RingBuffer_Ok;
}

RingBuffer_State_e USART2_ReadChar(uint8_t* data) {
    if (ring_buffer_is_empty(&rx_ring_buffer)) {
        return RingBuffer_Empty;
    }
    
    ring_buffer_pop(&rx_ring_buffer, data);
    return RingBuffer_Ok;
}
