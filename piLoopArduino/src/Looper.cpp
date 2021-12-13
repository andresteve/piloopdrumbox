#include "Looper.h"
#include "Keypad.h"


Looper::Looper(Keypad* drumpad, Keypad* trackpad, HardwareSerial* s, double baudRate){
    _drumpad = drumpad;
    _trackpad = trackpad;
    _serial = s;
    _baudRate = baudRate;
}

void Looper::init(){
    _serial->begin(_baudRate);
    _drumpad->init();
    _trackpad->init();
    for(uint8_t i=0; i<LOOP_TRACKS; i++){
        _tracks[i] = EMPTY;
    }
}

//Send data to Raspberry though serial
//Using the already defined PureData protocol
void Looper::sendDataToPi(byte msg, byte value){
    _serial->print(msg);
    _serial->print(" ");
    _serial->print(value);
    _serial->print(" ");
    _serial->println("");
}


//Update drumpad buttons
void Looper::updateDrumpad(){
    // Update drumPad
    uint8_t msgValue = IDLE;
    if (_drumpad->getKeys()){
        for (uint8_t i=0; i< (_drumpad->getNumberKeys()) ; i++){         // Scan the whole key list.
            Key k = _drumpad->keys[i];
            if ( k.stateChanged && (k.state != IDLE)){                  // Only find keys that have changed state.
                sendDataToPi(k.id, k.state);
           }
        }
    }
}

//Update trackpad buttons
void Looper::updateTrackpad(){
    // Update trackpad
    if (_trackpad->getKeys()){
        for (uint8_t i=0; i< (_trackpad->getNumberKeys()) ; i++){       // Scan the whole key list.
            Key k = _trackpad->keys[i];
            Track t = _tracks[i];
            if ( k.stateChanged && ( k.state != IDLE)){                 // Only find keys that have changed state.
                sendDataToPi(k.id, k.state);
                switch(t){                                              // Change track state
                    case EMPTY:
                        if(k.state == PRESSED)  t = REC;
                        break;
                    case REC:
                        if(k.state == PRESSED)  t = PLAY;
                        break;
                    case PLAY:
                        if(_muteKey.state == PRESSED && k.state == PRESSED )      t = MUTE;          // ISSUE: No exit from overdub state
                        else if (k.state == PRESSED)    t = OVERDUB;                                   
                        else if (k.state == HOLD)       t = EMPTY;                                   // Clear single track 
                        break;
                    case OVERDUB:
                        break;
                    case MUTE:
                        if(k.state == PRESSED)      t = PLAY;
                        break;
                }
           }
        }
    }
}


void Looper::debugKey(Key k){
    String msg = "IDLE";
    Serial.print("Button ID: "); Serial.print( k.id);
    Serial.print(" state: "); 
    switch(k.state){
        case IDLE:  
            msg = "IDLE"; 
            break;
        case PRESSED:  
            msg = "PRESSED"; 
            break;
        case HOLD:  
            msg = "HOLD"; 
            break;
        case RELEASED:  
            msg = "RELEASED"; 
            break;
    }
    Serial.print(msg);
    Serial.print(" state changed: "); Serial.print( k.stateChanged);
    Serial.print(" actual value: "); Serial.print( k.actualValue);
    Serial.println("");
}