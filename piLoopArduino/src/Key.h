
#ifndef _KEY_H_
#define _KEY_H_

#include <Arduino.h>

typedef enum{ RELEASED, PRESSED, HOLD } KeyState;

const uint8_t NO_KEY = 255;
const uint8_t NO_LED = 255;

/**
 * @brief This class control a single key (button)
 * 
 */
class Key {
	public:
		uint8_t id;
		uint8_t pin;
		uint8_t idLed;
		KeyState state;
		boolean stateChanged;
		uint8_t actualValue;

		Key();
		Key(uint8_t keyId, uint8_t keyPin, uint8_t keyLed);
		void keyUpdate(KeyState newState);
};

#endif


