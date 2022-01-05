#include "Key.h"


Key::Key(){
	id = NO_KEY;
	pin = 0;
	idLed = NO_LED;
	state = RELEASED;
	stateChanged = false;
	_holdTimer = 0;
}

Key::Key(uint8_t keyId, uint8_t keyPin, uint8_t keyLed){
	id = keyId;
	pin = keyPin;
	idLed = keyLed;
	state = RELEASED;
	stateChanged = false;
	_holdTimer = 0;
}

void Key::init(uint32_t pinMod){
	pinMode(pin, pinMod);
}


void Key::changeState(KeyState newState){
	if(newState != state)	stateChanged = true;
	else					stateChanged = false;
	state = newState;
}

bool Key::update(uint8_t pinToRead){
	KeyState newState = state;
	actualValue = !digitalRead(pinToRead);							    // Read button actual value. Low value measn button pressed
	switch (state) {
		case RELEASED:
			if (actualValue == PRESSED) {								// Get ready for next _hold state.
				newState = PRESSED;
				_holdTimer = millis(); 
			}								
			break;
		case PRESSED:
			if ((millis()-_holdTimer) > HOLD_TIME)						// Waiting for a key _hold...
				newState = HOLD;
			else if(actualValue == RELEASED)				    		// or for a key to be RELEASED.
				newState = RELEASED;
			break;
		case HOLD:
			if (actualValue == RELEASED)
				newState = RELEASED;
			break;
	}
	changeState(newState);
	return stateChanged;
}


/**
 * @brief Serial debug. Key status
 * 
 * @param k Selected key
 */
void Key::serialDebug(){
    String msg = "IDLE";
    Serial.print("Button ID: "); Serial.print( id);
    Serial.print(" state: "); 
    switch(state){
         case RELEASED:  
            msg = "RELEASED"; 
            break;
        case PRESSED:  
            msg = "PRESSED"; 
            break;
        case HOLD:  
            msg = "HOLD"; 
            break;
    }
    Serial.print(msg);
    Serial.print(" state changed: "); Serial.print( stateChanged);
    Serial.print(" actual value: "); Serial.print( actualValue);
    Serial.println("");
}