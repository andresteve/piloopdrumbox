#include <Keypad.h>


Keypad::Keypad(uint8_t *idKey, uint8_t *idLed, uint8_t *rowPins, uint8_t *colPins, uint8_t numRows, uint8_t numCols){
	_rowPins = rowPins;
	_colPins = colPins;
	_nRows = numRows;
	_nCols = numCols;
	_idKey = idKey;
	_idLed = idLed;
	_debounceTime = DEBOUNCE_TIME;
	_holdTime = HOLD_TIME;

	_startTime = 0;
	keys = (Key*) realloc (keys, (_nRows * _nCols) * sizeof(Key)); //Initialize array of all keys
	
}


// Initialize rows in OUTPUT and columns as INPUT
// Initialize key objects
void Keypad::init(){
	for (uint8_t r=0; r<_nRows; r++) {
		pinMode(_rowPins[r],OUTPUT);
	}
	for (uint8_t c=0; c<_nCols; c++) {
		pinMode(_colPins[c],INPUT_PULLUP);
	}
	for(uint8_t i=0; i<(_nRows * _nCols); i++){
		keys[i].id = _idKey[i];
		keys[i].idLed = _idLed[i];
		keys[i].actualValue = 0;
		keys[i].state = IDLE;
		keys[i].stateChanged = false;
	}
}


// Populate the key list.
bool Keypad::getKeys() {
	bool keyActivity = false;
	// Limit how often the keypad is scanned. 
	if ( (millis() - _startTime) > _debounceTime ) {
		scanKeys();
		keyActivity = updateList();
		_startTime = millis();
	}
	return keyActivity;
}

// Private : Hardware scan
void Keypad::scanKeys() {
	// bitMap stores ALL the keys that are being pressed.
	uint8_t idx = 0;
	for (uint8_t r=0; r<_nRows; r++ ) {
		digitalWrite(_rowPins[r], LOW);	// Begin row pulse output.
		for (uint8_t c=0; c<_nCols; c++) {
			keys[idx].actualValue = !digitalRead(_colPins[c]);  // keypress is active low so invert to high.	
			idx++;
		}
		// Set pin to high impedance input. Effectively ends row pulse.
		digitalWrite(_rowPins[r], HIGH);
	}
}

bool Keypad::updateList() {
	bool anyActivity = false;
	for(uint8_t i=0; i<(_nRows * _nCols); i++){
		keys[i].keyUpdate(keys[i].state);									// Reset stateChanged variable
		switch (keys[i].state) {
			case IDLE:
				if (keys[i].actualValue == PRESSED) {
					keys[i].keyUpdate(PRESSED);
					_holdTimer = millis(); }								// Get ready for next HOLD state.
				break;
			case PRESSED:
				if ((millis()-_holdTimer) > _holdTime)						// Waiting for a key HOLD...
					keys[i].keyUpdate(HOLD);
				else if(keys[i].actualValue == RELEASED)				// or for a key to be RELEASED.
					keys[i].keyUpdate(RELEASED);
				break;
			case HOLD:
				if (keys[i].actualValue == RELEASED)
					keys[i].keyUpdate(RELEASED);
				break;
			case RELEASED:
				keys[i].keyUpdate(IDLE);
				break;
		}
		if(keys[i].stateChanged)	anyActivity = true;						//If at least one key changed its status return true
	}
	return anyActivity;
}

int Keypad::getNumberKeys(){
	return _nCols * _nRows;
}
