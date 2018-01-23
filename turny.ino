/*
Turny: An alternative video game controller using four potentiometers
by mit-mit

This code runs on a Adafruit Bluefruit M0 Featherboard which is connected to 
four potentiometers (via analog in pins), a single momentary button and a
single neopixel. Data on the position of the pots and on/off events relating to 
the button are sent to a host computer via bluetooth LE.

This code uses the Adafruit Neopixel and Bluefruit LE libraries, see:
https://github.com/adafruit/Adafruit_NeoPixel
https://github.com/adafruit/Adafruit_BluefruitLE_nRF51

*/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#include <Adafruit_NeoPixel.h>

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

// Bluefruit LE settings
#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

// Setup analog read on pins 0,1,2,3
#define POT0_PIN 0
#define POT1_PIN 1
#define POT2_PIN 2
#define POT3_PIN 3

int pot0_value = 0;
int pot1_value = 0;
int pot2_value = 0;
int pot3_value = 0;

uint8_t valc0, valc1, valc2, valc3;
uint8_t valc_array[6];

// button
#define BUTTON_PIN 5

// Setup neopixel
#define LED_PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);

// Light show variables
int lshow_to = 0;

// setPixelHue - Quick function to balance RGB channels to create a particular colour
void setPixelHue(int pixel, int hue)
{
  while(hue < 0)
  {
    hue += 360;
  }
  
  float h = hue % 360;
    
  float sectorPos = h / 60;
  int sectorNumber = (int)floor(sectorPos);
  float fractionalSector = sectorPos - sectorNumber;
        
  float q = 1 - fractionalSector;
  float t = fractionalSector;
        
  switch(sectorNumber)
  {
    case 0:
      strip.setPixelColor(pixel, 255, 255 * t, 0);
      break;
    case 1:
      strip.setPixelColor(pixel, 255 * q, 255, 0);
      break;
    case 2:
      strip.setPixelColor(pixel, 0, 255, 255 * t);
      break;
    case 3:
      strip.setPixelColor(pixel, 0, 255 * q, 255);
      break;
    case 4:
      strip.setPixelColor(pixel, 255 * t, 0, 255);
      break;
    case 5:
      strip.setPixelColor(pixel, 255, 0, 255 * q);
      break;
  }
}

// Create the bluefruit object: hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// setup: Connect to a host via bluetooth
void setup(void)
{

  // Initialise neopixel
  strip.begin();
  strip.setBrightness(255);
  for (int i = 0; i < 360; i+=2) {
    setPixelHue(0, i);
    strip.show();
    delay(5);
  }
  setPixelHue(0, 0);
  strip.show();

  // Initialise button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialise and wait for connection
  ble.begin(0);
  ble.factoryReset();
  ble.echo(false);
  ble.verbose(false);
  while (! ble.isConnected()) {
      setPixelHue(0, 60);
      strip.show();
      delay(500);
      /*for (int i = 255; i >= 0; i++) {
        strip.setBrightness(i);
        strip.show();
        delay(1);
      }
      for (int i = 1; i < 255; i++) {
        strip.setBrightness(i);
        strip.show();
        delay(1);
      }*/
  }

  // Connected
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) ) {
    // Change Mode LED Activity
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set module to DATA mode
  ble.setMode(BLUEFRUIT_MODE_DATA);

  // set to connected
  strip.setBrightness(255);
  setPixelHue(0, 120);
  strip.show();
  
}

// loop: Stream data and wait for LED commands
void loop(void)
{

  // Read pot values, convert to 8bit
  pot0_value = analogRead(A0);
  pot1_value = analogRead(A1);
  pot2_value = analogRead(A2);
  pot3_value = analogRead(A3);
  valc0 = (uint8_t)(((float)pot0_value)/4.0);
  valc1 = (uint8_t)(((float)pot1_value)/4.0);
  valc2 = (uint8_t)(((float)pot2_value)/4.0);
  valc3 = (uint8_t)(((float)pot3_value)/4.0);

  // Send controller state packet
  valc_array[0] = 33;
  valc_array[1] = valc0;
  valc_array[2] = valc1;
  valc_array[3] = valc2;
  valc_array[4] = valc3;
  if (digitalRead(BUTTON_PIN)) {
    valc_array[5] = 1;
  }
  else {
    valc_array[5] = 0;
  }
  ble.write(valc_array,6);
  
  // Check for led commands
  while ( ble.available() ) {
    char c = ble.read();
    lshow_to = 360;
  }

  // Check for light show
  if (lshow_to > 0) {
    setPixelHue(0, lshow_to);
    strip.show();
    lshow_to -= 10;
  }
  else {
    setPixelHue(0, 120);
    strip.show();
  }
  
  delay(40);
  
}

