#include "Encoder.h"

/**
 * @brief Construct a new Encoder object
 * 
 * @param pinCLK Pin signal CLK
 * @param pinDT Pin signal DT
 * @param pinSW Pin switch button
 * @param ppr Pulse Per Revolution
 * @param pprDivider Pulse Per Revolution divider
 */
Encoder::Encoder(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW, long ppr, uint8_t pprDivider){
    _pinCLK = pinCLK;
    _pinDT = pinDT;
    _pinSW = pinSW;
    _ppr = ppr;
    _pprDivider = pprDivider;
}

/**
 * @brief Initialize Encoder object. 
 *        Setting the pin as INPUT. Reset pulse count.
 * 
 */
void Encoder::init(){
    pinMode(_pinCLK, INPUT);
    pinMode(_pinDT, INPUT);
    pinMode(_pinSW,INPUT);
    _lastStateCLK = digitalRead(_pinCLK);
    _count = 0;
}


/**
 * @brief Read input pin and update pulse counter and direction.
 *        It count from 0 to ppr.
 */
void Encoder::updateEncoder(){
	_currentStateCLK = digitalRead(_pinCLK);
	if (_currentStateCLK != _lastStateCLK  && _currentStateCLK == 1){
		if (digitalRead(_pinDT) != _currentStateCLK) {
			_count --;
            _dir = 0;
		} else {
			_count ++;
            _dir = 1;
		}
        if(_count > _ppr)    _count = 0;
        else if(_count < 0)  _count = _ppr;
    }

    if(!digitalRead(_pinSW))    _pressed = true;
    else                        _pressed = false;

	_lastStateCLK = _currentStateCLK;
}

/**
 * @brief Return counted pulse.
 * 
 * @return uint8_t Number of pulses
 */
uint8_t Encoder::getPulses(){
    return (uint8_t) _count/_pprDivider;
}

/**
 * @brief Get spin direction
 * 
 * @return 0: CW    1:CCW
 */
uint8_t Encoder::getDirection(){
    return _dir;
}

/**
 * @brief Return value of switch button
 * 
 * @return bool
 */
bool Encoder::isPressed(){
    return _pressed;
}