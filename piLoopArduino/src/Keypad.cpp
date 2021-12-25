#include <Keypad.h>


/**
 * @brief Construct a new Keypad
 * 
 * @param idKey Array of all buttons IDs
 * @param idLed Array of all leds IDs.  
 * @param rowPins Array of all pins connected to the rows of the matrix
 * @param colPins Array of all pins connected to the colums of the matrix
 * @param numRows Number of rows in the matrix
 * @param numCols Number of columns in the matrix
 */
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



/**
 * @brief Initialize keypad object.
 * 	      Initialize rows as OUTPUT and columns as INPUT_PULLUP.
 * 		  Initialize key object.
 * 
 */
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
		keys[i].state = RELEASED;
		keys[i].stateChanged = false;
	}
}


/**
 * @brief Scan keys every DEBOUNCE_TIME ms.
 * 		  Return true if any of these are pressed.
 * 
 * @return bool
 */
bool Keypad::getKeys() {
	bool keyActivity = false;
	if ( (millis() - _startTime) > _debounceTime ) { 	// Limit how often the keypad is scanned. 
		scanKeys();
		keyActivity = updateList();
		_startTime = millis();
	}
	return keyActivity;
}

/**
 * @brief Scan keys of the matrix. Update status of each key.
 * 
 */
void Keypad::scanKeys() {
	uint8_t idx = 0;
	for (uint8_t r=0; r<_nRows; r++ ) {
		digitalWrite(_rowPins[r], LOW);							// Begin row pulse output.
		for (uint8_t c=0; c<_nCols; c++) {
			keys[idx].actualValue = !digitalRead(_colPins[c]);  // Change Key staus. Keypress is active low so invert to high.	
			idx++;
		}
		digitalWrite(_rowPins[r], HIGH);						// Set pin to high impedance input. Effectively ends row pulse.
	}
}

/**
 * @brief Change key status depending on actual and previous one.
 * 		  Return true if there is any changes.
 * 
 * @return bool
 */
bool Keypad::updateList() {
	bool anyActivity = false;
	for(uint8_t i=0; i<(_nRows * _nCols); i++){
		keys[i].keyUpdate(keys[i].state);									// Reset stateChanged variable
		switch (keys[i].state) {
			case RELEASED:
				if (keys[i].actualValue == PRESSED) {
					keys[i].keyUpdate(PRESSED);
					_holdTimer = millis(); }								// Get ready for next HOLD state.
				break;
			case PRESSED:
				if ((millis()-_holdTimer) > _holdTime)						// Waiting for a key HOLD...
					keys[i].keyUpdate(HOLD);
				else if(keys[i].actualValue == RELEASED)				    // or for a key to be RELEASED.
					keys[i].keyUpdate(RELEASED);
				break;
			case HOLD:
				if (keys[i].actualValue == RELEASED)
					keys[i].keyUpdate(RELEASED);
				break;
		}
		if(keys[i].stateChanged)	anyActivity = true;						//If at least one key changed its status return true
	}
	return anyActivity;
}


/**
 * @brief Return number of keys
 * 
 * @return int columns*rows
 */
int Keypad::getNumberKeys(){
	return _nCols * _nRows;
}
