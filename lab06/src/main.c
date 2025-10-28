/*
File: Lab_6_ESP8266_WebServer_DS1722.c
Author: Emmett Stralka
Email: emmettstralka@hmc.edu
Date: 12/19/2024
Description: ESP8266 web server with DS1722 temperature sensor integration
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "DS1722.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";

char* resolutionStr = "<h2>Temperature Resolution</h2>\
	<p>Current Resolution: <span id=\"currentRes\">12-bit</span></p>\
	<form action=\"res8\"><input type=\"submit\" value=\"8-bit (1.0&deg;C)\"></form>\
	<form action=\"res9\"><input type=\"submit\" value=\"9-bit (0.5&deg;C)\"></form>\
	<form action=\"res10\"><input type=\"submit\" value=\"10-bit (0.25&deg;C)\"></form>\
	<form action=\"res11\"><input type=\"submit\" value=\"11-bit (0.125&deg;C)\"></form>\
	<form action=\"res12\"><input type=\"submit\" value=\"12-bit (0.0625&deg;C)\"></form>";
char* webpageEnd   = "</body></html>";

// Check if character sequence exists in request array
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[], int current_state)
{
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		current_state = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		current_state = 1;
	}
	return current_state;
}

// Decode temperature from SPI bits
float readTemperatureSPI(uint8_t msb, uint8_t lsb){
    // Combine MSB and LSB to get 12-bit temperature
    int16_t raw_temp = (msb << 8) | lsb;
    
    // Convert to temperature in Celsius
    // DS1722 uses 12-bit two's complement with 0.0625°C per LSB
    float temperature = (float)raw_temp * 0.0625;
    
    return temperature;
}

// Read temperature from DS1722 via SPI
float updateTemperature(char request[]) {
    uint8_t msb, lsb;
    
    // DS1722 temperature read sequence:
    // 1. Start conversion (if needed)
    // 2. Read temperature registers
    
    // Read LSB (register 0x01) - DS1722 uses 0x01 for LSB
    digitalWrite(PA11, PIO_HIGH);
    spiSendReceive(0x01); // Read LSB register
    lsb = spiSendReceive(0x00); // Dummy byte to get LSB data
    digitalWrite(PA11, PIO_LOW);
    
    // Small delay between transactions
    delayMicroseconds(100);
    
    // Read MSB (register 0x02) - DS1722 uses 0x02 for MSB  
    digitalWrite(PA11, PIO_HIGH);
    spiSendReceive(0x02); // Read MSB register
    msb = spiSendReceive(0x00); // Dummy byte to get MSB data
    digitalWrite(PA11, PIO_LOW);

    return readTemperatureSPI(msb, lsb);
}

// Handle resolution changes
int updateResolution(char request[]) {
    static int current_resolution = 12; // Track current resolution
    
    // Apply resolution changes to DS1722
    if (inString(request, "res8") == 1) {
        digitalWrite(PA11, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100000);
        digitalWrite(PA11, PIO_LOW);
        current_resolution = 8;
    } else if (inString(request, "res9") == 1) {
        digitalWrite(PA11, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100010);
        digitalWrite(PA11, PIO_LOW);
        current_resolution = 9;
    } else if (inString(request, "res10") == 1) {
        digitalWrite(PA11, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100100);
        digitalWrite(PA11, PIO_LOW);
        current_resolution = 10;
    } else if (inString(request, "res11") == 1) {
        digitalWrite(PA11, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11100110);
        digitalWrite(PA11, PIO_LOW);
        current_resolution = 11;
    } else if (inString(request, "res12") == 1) {
        digitalWrite(PA11, PIO_HIGH);
        spiSendReceive(0x80);
        spiSendReceive(0b11101000);
        digitalWrite(PA11, PIO_LOW);
        current_resolution = 12;
    }
    
    return current_resolution;
}





//Main Functions

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(LED_PIN, GPIO_OUTPUT);
  digitalWrite(LED_PIN, PIO_LOW); // Initialize LED off
  
  // Initialize timer for delays
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  initTIM(TIM6);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

   // Initialize hardware SPI for DS1722
   initSPI(0b111, 0, 0); // br=7 (slowest), cpol=0, cpha=0 for DS1722 (try Mode 0)
   
   // Configure DS1722 CS pin
   pinMode(PA11, GPIO_OUTPUT);
   digitalWrite(PA11, PIO_LOW);
   
   // Wait for DS1722 to power up
   delayMicroseconds(1000);
   
   // Initialize DS1722: Set 12-bit resolution and continuous conversion
   digitalWrite(PA11, PIO_HIGH);
   spiSendReceive(0x80); // Write to configuration register (0x80)
   spiSendReceive(0xE8); // 12-bit resolution, continuous conversion (0xE8)
   digitalWrite(PA11, PIO_LOW);
   
   // Wait for configuration to take effect
   delayMicroseconds(1000);
   
   // Alternative: Try reading configuration to verify
   digitalWrite(PA11, PIO_HIGH);
   spiSendReceive(0x80); // Read configuration register
   uint8_t config = spiSendReceive(0x00); // Read config value
   digitalWrite(PA11, PIO_LOW);
   
   // Start temperature conversion
   digitalWrite(PA11, PIO_HIGH);
   spiSendReceive(0x51); // Start conversion command
   digitalWrite(PA11, PIO_LOW);
   
   // Wait for first conversion to complete
   delayMicroseconds(1000);

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // Read temperature from DS1722 sensor
    float temperature = updateTemperature(request);
  
    // Update LED state and resolution
    static int led_status = 0; // Initialize LED status
    led_status = updateLEDStatus(request, led_status);
    int current_resolution = updateResolution(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");
    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    // Display temperature reading with current resolution
    sendString(USART, "<h2>Temperature</h2>");
    sendString(USART, "<p>Current Temperature: ");
    
    char tempStr[40];
    sprintf(tempStr, "%.4f", temperature);
    sendString(USART, tempStr);
    sendString(USART, " &deg;C</p>");
    
    // Display current resolution
    char resStr[40];
    sprintf(resStr, "<p>Current Resolution: %d-bit</p>", current_resolution);
    sendString(USART, resStr);
    
    // Add resolution control buttons
    sendString(USART, resolutionStr);
  
    sendString(USART, webpageEnd);
  }
}