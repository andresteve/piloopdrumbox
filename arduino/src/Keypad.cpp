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
	_scanTime = 0;
	keys = (Key*) realloc(keys,(_nRows * _nCols) * sizeof(Key));
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
	for(uint8_t i=0; i<(_nRows * _nCols); i++){				//Initialize array of all keys
		keys[i]=Key(_idKey[i], NO_KEY, _idLed[i]);
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
	if ( (millis() - _scanTime) > DEBOUNCE_TIME ) { 	// Limit how often the keypad is scanned. 
		keyActivity = scanKeys();
		_scanTime = millis();
	}
	return keyActivity;
}

/**
 * @brief Scan keys of the matrix. Update status of each key.
 * 
 */
bool Keypad::scanKeys() {
	uint8_t idx = 0;
	bool anyActivity = false, activity = false;
	for (uint8_t r=0; r<_nRows; r++ ) {
		digitalWrite(_rowPins[r], LOW);							// Begin row pulse output.
		for (uint8_t c=0; c<_nCols; c++) {
			activity = keys[idx].update(_colPins[c]);  			// Change Key staus. Keypress is active low so invert to high.
			if(activity)	anyActivity = true;
			idx++;
		}
		digitalWrite(_rowPins[r], HIGH);						// Set pin to high impedance input. Effectively ends row pulse.
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


uint8_t Keypad::getNumbersRows(){
	return _nRows;
}


uint8_t Keypad::getNumberColumns(){
	return _nCols;
}