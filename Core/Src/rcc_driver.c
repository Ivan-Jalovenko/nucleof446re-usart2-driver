#include "rcc_driver.h"
#include "stm32f446xx.h"
#include "stm32f4xx.h"


RCC_Presc_e RCC_AHBPresc() {
    uint32_t cfgr = RCC->CFGR;

    if ((cfgr & RCC_CFGR_HPRE_3) == 0) { return RCC_DIV1; }

    switch(RCC->CFGR & RCC_CFGR_HPRE) {
        case RCC_CFGR_HPRE_DIV2:    return RCC_DIV2;
        case RCC_CFGR_HPRE_DIV4:    return RCC_DIV4;
        case RCC_CFGR_HPRE_DIV8:    return RCC_DIV8;
        case RCC_CFGR_HPRE_DIV16:   return RCC_DIV16;
        case RCC_CFGR_HPRE_DIV64:   return RCC_DIV64;
        case RCC_CFGR_HPRE_DIV128:  return RCC_DIV128;
        case RCC_CFGR_HPRE_DIV256:  return RCC_DIV256;
        case RCC_CFGR_HPRE_DIV512:  return RCC_DIV512;
        default:                    return RCC_Invalid;
    }
}

RCC_Presc_e RCC_APB1Presc() {
    uint32_t cfgr = RCC->CFGR;

    if ((cfgr & RCC_CFGR_PPRE1_2) == 0) { return RCC_DIV1; } 

    switch(RCC->CFGR & RCC_CFGR_PPRE1) {
        case RCC_CFGR_PPRE1_DIV2:   return RCC_DIV2;
        case RCC_CFGR_PPRE1_DIV4:   return RCC_DIV4;
        case RCC_CFGR_PPRE1_DIV8:   return RCC_DIV8;
        case RCC_CFGR_PPRE1_DIV16:  return RCC_DIV16;
        default:                    return RCC_Invalid;
    }
}
