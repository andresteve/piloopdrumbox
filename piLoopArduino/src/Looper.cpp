#include "Looper.h"
#include "Keypad.h"


Looper::Looper(Keypad* k, HardwareSerial* s, double baudRate){
    _keypad = k;
    _serial = s;
    _baudRate = baudRate;
}

void Looper::init(){
    _serial->begin(_baudRate);
}

//Send data to Raspberry though serial
//Using the already defined PureData protocol
void Looper::sendDataToPi(String msg, int value){
    _serial->print(msg);
    _serial->print(" ");
    _serial->print(value);
    _serial->print(" ");
    _serial->println("");
}


//Update buttons
void Looper::updateButtons(){

    // Update drumPad
    if (_keypad->getKeys()){
        for (int i=0; i<LIST_MAX; i++){   // Scan the whole key list.
            if ( _keypad->key[i].stateChanged ){  // Only find keys that have changed state.
                switch (_keypad->key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:
                        sendDataToPi(_keypad->key[i].kstr,1);
                        break;
                    case HOLD:
                        break;
                    case RELEASED:
                        sendDataToPi(_keypad->key[i].kstr,0);
                    case IDLE:
                        break;
                }
            }
        }
    }
}