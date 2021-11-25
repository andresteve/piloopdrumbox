#include <Arduino.h>
#include "Keypad.h"
#include "Looper.h"

/*** SERIAL CONFIG ***/
#define SR0_BAUD_RATE             9600      // Serial 0 used for debug
#define SERIAL_TO_PI_BAUD_RATE    9600

// BUG
#define SERIAL_TO_PI                1       // Serial 1 communicate with Raspberry (Choose between serial 1-2-3)


/*** DRUM PAD CONFIG ***/
#define DRUM_PAD_ROWS 4 //four rows
#define DRUM_PAD_COLS 4 //four columns
byte rowPins[DRUM_PAD_ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[DRUM_PAD_COLS] = {5, 4, 3, 2};//connect to the column pinouts of the keypad


/*** REC BUTTONS CONFIG ***/
#define REC_BUTTONS 4
//byte recPins[REC_BUTTONS] = {}

String drumPadButtons[DRUM_PAD_ROWS][DRUM_PAD_COLS] = {
  {"Dr1", "Dr2", "Dr3", "Dr4"},
  {"Dr5", "Dr6", "Dr7", "Dr8"},
  {"Dr9", "Dr10", "Dr11", "Dr12"},
  //{"Dr13", "Dr14", "Dr15", "Dr16"} original
  {"R1", "R2", "R3", "R4"}
};

String recButtons[REC_BUTTONS] = {"R1", "R2", "R3", "R4"};

Keypad keypad = Keypad( makeKeymap(drumPadButtons), rowPins, colPins, DRUM_PAD_ROWS, DRUM_PAD_COLS );

Looper looper = Looper(&keypad, &Serial1, SERIAL_TO_PI_BAUD_RATE);



void setup() {
  Serial.begin(SR0_BAUD_RATE);
  looper.init();
}

void loop() {
  looper.updateButtons();
}