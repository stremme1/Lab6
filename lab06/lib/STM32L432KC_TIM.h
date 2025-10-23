// STM32L432KC_TIM.h
// Header for TIM functions

#ifndef STM32L432KC_TIM_H
#define STM32L432KC_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>  // CMSIS device library include
#include "STM32L432KC_GPIO.h"


///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx);
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms);

#endif