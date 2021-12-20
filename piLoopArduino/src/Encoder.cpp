#include "Encoder.h"

Encoder::Encoder(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW, long ppr, uint8_t pToSelect){
    _pinCLK = pinCLK;
    _pinDT = pinDT;
    _pinSW = pinSW;
    _ppr = ppr;
    _pToSelect = pToSelect;
}

void Encoder::init(){
    pinMode(_pinCLK, INPUT);
    pinMode(_pinDT, INPUT);
    pinMode(_pinSW,INPUT);
    _lastStateCLK = digitalRead(_pinCLK);
    _count = 0;
}

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

uint8_t Encoder::getPulses(){
    return (uint8_t) _count/_pToSelect;
}

bool Encoder::isPressed(){
    return _pressed;
}