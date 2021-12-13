#include "Key.h"


Key::Key(){
}

Key::Key(uint8_t keyId, uint8_t keyPin, uint8_t keyLed){
	id = keyId;
	pin = keyPin;
	idLed = keyLed;
	state = IDLE;
	stateChanged = false;
}


void Key::keyUpdate(KeyState newState){
	if(newState != state)	stateChanged = true;
	else					stateChanged = false;
	state = newState;
}




