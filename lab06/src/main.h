/**
    Main Header: Contains general defines and selected portions of CMSIS files
    @file main.h
    @author Josh Brake
    @version 1.0 10/7/2020
*/

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_USART.h"
#include "STM32L432KC_SPI.h"

#define LED_PIN PA5 // LD3 LED pin on STM32L432KC (Port A pin 5)
#define BUFF_LEN 32

// Function prototypes for main.c
int inString(char request[], char des[]);
int updateLEDStatus(char request[], int current_state);
int updateResolution(char request[]);
float readTemperatureSPI(uint8_t msb, uint8_t lsb);
float updateTemperature(char request[]);

#endif // MAIN_H