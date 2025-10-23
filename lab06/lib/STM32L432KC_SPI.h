/*
STM32L432KC_SPI.h
Author: Emmett Stralka
Date: 12/19/2024
Description: SPI peripheral driver for STM32L432KC
*/

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>

// SPI Pin Definitions
#define SPI_CE PA11
#define SPI_SCK PB3
#define SPI_MOSI PB5
#define SPI_MISO PB4

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Initialize SPI peripheral with specified clock speed, polarity, and phase
 * br: clock divider (0b000 - 0b111)
 * cpol: clock polarity (0 = low when idle, 1 = high when idle)
 * cpha: clock phase (0 = sample on first edge, 1 = sample on second edge) */ 
void initSPI(int br, int cpol, int cpha);

/* Send and receive data over SPI
 * send: byte to transmit
 * return: byte received from slave */
char spiSendReceive(char send);

#endif