#ifndef DS1722_H
#define DS1722_H

#include "STM32L432KC.h"

// Simple DS1722 implementation - Using Lab7 pins (same as hardware SPI)
#define DS1722_CS_PIN   PA11  // Chip Select (Lab7 pin)
#define DS1722_SCK_PIN  PB3   // Clock (Lab7 pin)
#define DS1722_MISO_PIN PB4   // Data In (Lab7 pin)
#define DS1722_MOSI_PIN PB5   // Data Out (Lab7 pin)

// Function prototypes
void DS1722_init(void);
float DS1722_read_temperature(void);
void DS1722_set_resolution(uint8_t resolution);
uint8_t DS1722_get_resolution(void);
void DS1722_set_baud_rate(uint8_t baud_rate);
uint8_t DS1722_get_baud_rate(void);

// Simple SPI function prototypes
void spi_delay(void);
uint8_t spi_transfer(uint8_t data);

#endif // DS1722_H