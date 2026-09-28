#ifndef RCC_DRIVER_H
#define RCC_DRIVER_H


#include <stdint.h>


typedef enum {
    RCC_Invalid  = 0,
    RCC_DIV1     = 1,
    RCC_DIV2     = 2,
    RCC_DIV4     = 4,
    RCC_DIV8     = 8,
    RCC_DIV16    = 16,
    RCC_DIV64    = 64,
    RCC_DIV128   = 128,
    RCC_DIV256   = 256,
    RCC_DIV512   = 512,
} RCC_Presc_e;

RCC_Presc_e RCC_AHBPresc();
RCC_Presc_e RCC_APB1Presc();

#endif // RCC_DRIVER_H
