#include "Key.h"


Key::Key(){
	id = NO_KEY;
	pin = 0;
	idLed = NO_LED;
	state = RELEASED;
	stateChanged = false;
}

Key::Key(uint8_t keyId, uint8_t keyPin, uint8_t keyLed){
	id = keyId;
	pin = keyPin;
	idLed = keyLed;
	state = RELEASED;
	stateChanged = false;
}


void Key::keyUpdate(KeyState newState){
	if(newState != state)	stateChanged = true;
	else					stateChanged = false;
	state = newState;
}




