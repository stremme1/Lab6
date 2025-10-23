// DS1722.c - SIMPLIFIED for Lab
#include "DS1722.h"

// DS1722 pin definitions
#define DS1722_CS_PIN   PA11  // Chip Select
#define DS1722_SCK_PIN  PB3   // Clock
#define DS1722_MISO_PIN PB4   // Data In
#define DS1722_MOSI_PIN PB5   // Data Out

// Global resolution setting
static uint8_t current_resolution = 12;
static uint8_t spi_baud_rate = 100;

// Simple delay
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

// Initialize DS1722
void DS1722_init(void) {
    // Configure CS pin
    pinMode(DS1722_CS_PIN, GPIO_OUTPUT);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
}

// Set resolution (8, 9, 10, 11, or 12 bits)
void DS1722_set_resolution(uint8_t resolution) {
    if (resolution < 8 || resolution > 12) return;
    current_resolution = resolution;
    
    // Set DS1722 resolution
    uint8_t config = 0x60;
    if (resolution == 8) config = 0b11100000;
    else if (resolution == 9) config = 0b11100010;
    else if (resolution == 10) config = 0b11100100;
    else if (resolution == 11) config = 0b11100110;
    else if (resolution == 12) config = 0b11101000;
    
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x80);
    spiSendReceive(config);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
}

// Read temperature
float DS1722_read_temperature(void) {
    uint8_t lsb, msb;
    
    // Read LSB
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x01);
    lsb = spiSendReceive(0x00);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
    
    // Read MSB
    digitalWrite(DS1722_CS_PIN, PIO_LOW);
    spiSendReceive(0x02);
    msb = spiSendReceive(0x00);
    digitalWrite(DS1722_CS_PIN, PIO_HIGH);
    
    // Convert to temperature
    float temperature = (msb & 0b01111111);

    // Check if temperature is negative
    if ((msb >> 7) & 1) {temperature -= 128;} 

    // Add resolution from LSB
    float resolution = (float)((lsb >> 4) & 0x0F) * 0.0625;

    return temperature + resolution;
}

// Get resolution
uint8_t DS1722_get_resolution(void) {
    return current_resolution;
}

// Set SPI baud rate
void DS1722_set_baud_rate(uint8_t baud_rate) {
    if (baud_rate > 0 && baud_rate <= 255) {
        spi_baud_rate = baud_rate;
    }
}

// Get SPI baud rate
uint8_t DS1722_get_baud_rate(void) {
    return spi_baud_rate;
}