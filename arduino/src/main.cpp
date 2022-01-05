/**
 * @file main.cpp
 * @author Stefano Andreoni
 * @brief Looper 
 * @version 0.1
 * @date 2021-12-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <FastLED.h>
#include "Keypad.h"
#include "Looper.h"
#include "Encoder.h"
#include "Track.h"
#include "TFT.h"

/*** SERIAL CONFIG ***/
#define SR0_BAUD_RATE             115200      // Serial 0 used for debug
#define SERIAL_TO_PI_BAUD_RATE    115200      // Serial 1 used to communicate with Raspberry


/*** DRUM PAD CONFIG ***/
#define DRUM_PAD_ROWS 4 //four rows
#define DRUM_PAD_COLS 4 //four columns
uint8_t drumRowPins[DRUM_PAD_ROWS] = {9, 8, 7, 6};  //connect to the row pinouts of the keypad
uint8_t drumColPins[DRUM_PAD_COLS] = {5, 4, 3, 2};  //connect to the column pinouts of the keypad
uint8_t drumpadKeyId[DRUM_PAD_ROWS * DRUM_PAD_COLS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //Assign key id .Start from (0,0) ... (0,1)
uint8_t drumpadLedId[DRUM_PAD_ROWS * DRUM_PAD_COLS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //Assign key id .Start from (0,0) ... (0,1)

/*** TRACK PAD CONFIG ***/
#define TRACK_PAD_ROWS 2 //four rows
#define TRACK_PAD_COLS 4 //four columns
#define TRACK_VOL_ANALOG_IN A0    // The tracks potentiometer are readed on the same pin an addressed with mux
#define TRACK_VOL_MUX_S0    50
#define TRACK_VOL_MUX_S1    51
#define TRACK_VOL_MUX_S2    52
#define MASTER_VOL_ANALOG_IN A1

uint8_t trackRowPins[TRACK_PAD_ROWS] = {27, 26}; //connect to the row pinouts of the keypad
uint8_t trackColPins[TRACK_PAD_COLS] = {25, 24, 23, 22};//connect to the column pinouts of the keypad
uint8_t trackpadKeyId[TRACK_PAD_ROWS * TRACK_PAD_COLS] = {16, 17, 18, 19, 20, 21, 22, 23}; //Assign key id .Start from (0,0) ... (0,1)
uint8_t trackpadLedId[TRACK_PAD_ROWS * TRACK_PAD_COLS] = {16, 17, 18, 19, 20, 21, 22, 23}; //Assign led id .Start from (0,0) ... (0,1)

/*** BUTTONS CONFIG ***/
#define MUTE_KEY_ID     24
#define MUTE_KEY_PIN    31
#define MUTE_KEY_LED_ID 24  

/*** LED CONFIG ***/
#define NUM_LEDS 24
#define LED_DATA_PIN 11

/*** ENCODER SELECTOR ***/
#define ENC_PIN_CLK 28
#define ENC_PIN_DT  29
#define ENC_PIN_SW  30
#define ENC_PPR     20
#define ENC_PPR_DIVIDER 1

/*** TFT SCREEN ***/
#define TFT_DC    40    //Use hardware SPI 
#define TFT_CS    42
#define TFT_RST   41



// Looper object
Keypad drumpadKeypad = Keypad( drumpadKeyId, drumpadLedId, drumRowPins, drumColPins, DRUM_PAD_ROWS, DRUM_PAD_COLS );
Keypad trackpadKeypad = Keypad( trackpadKeyId, trackpadLedId, trackRowPins, trackColPins, TRACK_PAD_ROWS, TRACK_PAD_COLS );
Key muteKey = Key(MUTE_KEY_ID, MUTE_KEY_PIN, MUTE_KEY_LED_ID);
CRGB leds[NUM_LEDS];
Encoder menuEncoder = Encoder(ENC_PIN_CLK, ENC_PIN_DT, ENC_PIN_SW, ENC_PPR, ENC_PPR_DIVIDER);
TFT tft = TFT(TFT_CS, TFT_DC, TFT_RST, &menuEncoder);
Track loopTracks[] ={Track(0, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(1, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(2, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(3, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(4, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(5, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(6, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true),
                     Track(7, TRACK_VOL_ANALOG_IN, TRACK_VOL_MUX_S0, TRACK_VOL_MUX_S1, TRACK_VOL_MUX_S2, true)};

Track loopMaster =  Track(9, MASTER_VOL_ANALOG_IN, 0, 0, 0, false);

Looper looper = Looper(&drumpadKeypad, &trackpadKeypad, loopTracks, &loopMaster, &tft, leds, &muteKey, &Serial1, SERIAL_TO_PI_BAUD_RATE); 


void setup() {
  Serial.begin(SR0_BAUD_RATE);
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  looper.init();
}

void loop() {
  looper.getDataFromPi();
  looper.update();
}