// STM32L432KC_SPI.h
// SPI peripheral driver for STM32L432KC
// Emmett Stralka
// Date: 12/19/2024

#ifndef STM32L4_SPI_H
#define STM32L4_SPI_H

#include <stdint.h>
#include <stm32l432xx.h>

// SPI pin definitions for DS1722 temperature sensor
#define SPI_CE PA11
#define SPI_SCK PB3
#define SPI_MOSI PB5
#define SPI_MISO PB4

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

/* Initialize SPI peripheral with specified baud rate, polarity, and phase
 * br: baud rate divisor (0b000-0b111), SPI clock = master clock / 2^(BR+1)
 * cpol: clock polarity (0=low when idle, 1=high when idle)
 * cpha: clock phase (0=capture on leading edge, 1=capture on trailing edge) */
void initSPI(int br, int cpol, int cpha);

/* Send and receive data over SPI
 * send: byte to transmit
 * return: byte received from slave */
char spiSendReceive(char send);

#endif