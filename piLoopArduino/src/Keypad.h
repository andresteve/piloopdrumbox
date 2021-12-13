#ifndef KEYPAD_H
#define KEYPAD_H

#include "Key.h"

#define DEBOUNCE_TIME 	10 // Not less than 1 ms
#define HOLD_TIME 		500



// bperrybap - Thanks for a well reasoned argument and the following macro(s).
// See http://arduino.cc/forum/index.php/topic,142041.msg1069480.html#msg1069480
#ifndef INPUT_PULLUP
#warning "Using  pinMode() INPUT_PULLUP AVR emulation"
#define INPUT_PULLUP 0x2
#define pinMode(_pin, _mode) _mypinMode(_pin, _mode)
#define _mypinMode(_pin, _mode)  \
do {							 \
	if(_mode == INPUT_PULLUP)	 \
		pinMode(_pin, INPUT);	 \
		digitalWrite(_pin, 1);	 \
	if(_mode != INPUT_PULLUP)	 \
		pinMode(_pin, _mode);	 \
}while(0)
#endif


class Keypad : public Key {
	public:
		Key *keys;
		Keypad(uint8_t *idKeyMap, uint8_t *idLedMap, uint8_t *row, uint8_t *col, uint8_t numRows, uint8_t numCols);
		void init();
		bool getKeys();
		int getNumberKeys();


	private:
		uint8_t *_idKey, *_idLed;
		uint8_t *_rowPins, *_colPins;
		uint8_t _nCols, _nRows;
		
		unsigned long _startTime, _holdTimer;
		unsigned long _debounceTime, _holdTime;

		void scanKeys();
		bool updateList();
};

#endif

