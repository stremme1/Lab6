// DS1722.c - SIMPLIFIED for Lab
#include "DS1722.h"

// DS1722 pin definitions - Using Lab7 hardware SPI pins
#define DS1722_CS_PIN   PA11  // Chip Select (Lab7 pin)
#define DS1722_SCK_PIN  PB3   // Clock (Lab7 pin)
#define DS1722_MISO_PIN PB4   // Data In (Lab7 pin)
#define DS1722_MOSI_PIN PB5   // Data Out (Lab7 pin)

// Global resolution setting (default 12-bit)
static uint8_t current_resolution = 12;
static uint8_t spi_baud_rate = 100; // Default delay for ~1MHz SPI

// Simple delay - adjustable for different baud rates
void spi_delay(void) {
    for(volatile int i = 0; i < spi_baud_rate; i++);
}

// Simple SPI transfer
uint8_t spi_transfer(uint8_t data) {
    uint8_t received = 0;
    
    for(int i = 7; i >= 0; i--) {
        // Set MOSI
        digitalWrite(DS1722_MOSI_PIN, (data & (1 << i)) ? PIO_HIGH : PIO_LOW);
        spi_delay();
        
        // Clock high
        digitalWrite(DS1722_SCK_PIN, PIO_HIGH);
        spi_delay();
        
        // Read MISO
        if (digitalRead(DS1722_MISO_PIN) == PIO_HIGH) {
            received |= (1 << i);
        }
        
        // Clock low
        digitalWrite(DS1722_SCK_PIN, PIO_LOW);
        spi_delay();
    }
    
    return received;
}

// Initialize DS1722 - Hardware SPI version
void DS1722_init(void) {
    // Only configure CS pin (hardware SPI handles SCK, MISO, MOSI)
    pinMode(DS1722_CS_PIN, GPIO_OUTPUT);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
}

// Set resolution (8, 9, 10, 11, or 12 bits)
void DS1722_set_resolution(uint8_t resolution) {
    if (resolution < 8 || resolution > 12) return;
    current_resolution = resolution;
    
    // Configure DS1722 with new resolution
    uint8_t config = 0x60; // Continuous mode
    if (resolution == 8) config = 0b11100000;
    else if (resolution == 9) config = 0b11100010;
    else if (resolution == 10) config = 0b11100100;
    else if (resolution == 11) config = 0b11100110;
    else if (resolution == 12) config = 0b11101000;
    
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x80); // Write config
    spiSendReceive(config);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
}

// Read temperature
float DS1722_read_temperature(void) {
    uint8_t lsb, msb;
    
    // Read LSB
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x01); // Read LSB register
    lsb = spiSendReceive(0x00);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
    
    // Read MSB
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x02); // Read MSB register
    msb = spiSendReceive(0x00);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
    
    // Convert to temperature
    float temperature = (msb & 0b01111111);

    // Check if temperature is negative
    if ((msb >> 7) & 1) {temperature -= 128;} 

    // Apply resolution based on LSB
    float resolution = (float)((lsb >> 4) & 0x0F) * 0.0625;

    return temperature + resolution;
}

// Get current resolution
uint8_t DS1722_get_resolution(void) {
    return current_resolution;
}

// Set SPI baud rate (1-255, higher = slower)
void DS1722_set_baud_rate(uint8_t baud_rate) {
    if (baud_rate > 0 && baud_rate <= 255) {
        spi_baud_rate = baud_rate;
    }
}

// Get current SPI baud rate
uint8_t DS1722_get_baud_rate(void) {
    return spi_baud_rate;
}