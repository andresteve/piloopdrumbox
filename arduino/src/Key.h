
#ifndef _KEY_H_
#define _KEY_H_

#include <Arduino.h>

typedef enum{ RELEASED, PRESSED, HOLD } KeyState;

const uint8_t NO_KEY = 255;
const uint8_t NO_LED = 255;
const uint16_t HOLD_TIME = 500;

/**
 * @brief This class control a single key (button)
 * 
 */
class Key {
	private:
		unsigned long _startTime, _holdTimer;
		
	public:
		uint8_t id, pin, idLed;
		KeyState state;
		boolean stateChanged;
		uint8_t actualValue;
		Key();
		Key(uint8_t keyId, uint8_t keyPin, uint8_t keyLed);
		void init(uint32_t pinMod);
		void changeState(KeyState newState);
		bool update(uint8_t pinToRead);
		void serialDebug();
};

#endif


