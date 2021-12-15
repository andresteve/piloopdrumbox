#include <Arduino.h>
#include "Keypad.h"
#include "Looper.h"

/*** SERIAL CONFIG ***/
#define SR0_BAUD_RATE             115200      // Serial 0 used for debug
#define SERIAL_TO_PI_BAUD_RATE    115200    // Serial 1 used to communicate with Raspberry



/*** DRUM PAD CONFIG ***/
#define DRUM_PAD_ROWS 4 //four rows
#define DRUM_PAD_COLS 4 //four columns
uint8_t drumRowPins[DRUM_PAD_ROWS] = {9, 8, 7, 6};  //connect to the row pinouts of the keypad
uint8_t drumColPins[DRUM_PAD_COLS] = {5, 4, 3, 2};  //connect to the column pinouts of the keypad
uint8_t drumpadKeyId[DRUM_PAD_ROWS * DRUM_PAD_COLS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //Assign key id .Start from (0,0) ... (0,1)
uint8_t *drumpadLedId = drumpadKeyId;

/*** TRACK PAD CONFIG ***/
#define TRACK_PAD_ROWS 2 //four rows
#define TRACK_PAD_COLS 4 //four columns
uint8_t trackRowPins[TRACK_PAD_ROWS] = {27, 26}; //connect to the row pinouts of the keypad
uint8_t trackColPins[TRACK_PAD_COLS] = {25, 24, 23, 22};//connect to the column pinouts of the keypad
uint8_t trackpadKeyId[TRACK_PAD_ROWS * TRACK_PAD_COLS] = {16, 17, 18, 19, 20, 21, 22, 23}; //Assign key id .Start from (0,0) ... (0,1)
uint8_t *trackpadLedId = trackpadKeyId;

Keypad drumpadKeypad = Keypad( drumpadKeyId, drumpadLedId, drumRowPins, drumColPins, DRUM_PAD_ROWS, DRUM_PAD_COLS );
Keypad trackpadKeypad = Keypad( trackpadKeyId, trackpadLedId, trackRowPins, trackColPins, TRACK_PAD_ROWS, TRACK_PAD_COLS );

Looper looper = Looper(&drumpadKeypad, &drumpadKeypad, &Serial1, SERIAL_TO_PI_BAUD_RATE);



void setup() {
  Serial.begin(SR0_BAUD_RATE);
  looper.init();
}

void loop() {
  looper.getDataFromPi();
  //looper.updateDrumpad();
  looper.updateTrackpad();
}