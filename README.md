# turny-controller
An alternative bluetooth video game controller based around four colour-coded dials.

by mit-mit

This code runs on a Adafruit Feather M0 Bluefruit LE board (Adafruit Feather M0 Bluefruit LE), an Arduino-compatible module consisting of a Cortex m0 micro-processor and low-energy bluetooth module. The controller reads the positions of four potentiometers and a single momentary button and streams this data across a bluetooth connection to a desktop/laptop computer running custom-designed games. The controller also has a single neopixel which is used to indicate bluetooth connection status and run blinky rainbow events when signalled by the connected game.

For details on the hardware and contruction process, see: TBC

Required libraries:

Adafruit Neopixels (used to run neopixel): https://github.com/adafruit/Adafruit_NeoPixel

Adafruit Bluefruit LE (used to run blluefruit featherboard): https://github.com/adafruit/Adafruit_BluefruitLE_nRF51

The "BluefruitConfig.h" header file is originally from examples provided in the Adafruit Bluefruit LE library.
