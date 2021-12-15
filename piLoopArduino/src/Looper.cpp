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
    for(uint8_t i=0; i<_trackpad->getNumberKeys(); i++){
        _trackState[i] = CLEAR_REC;
    }
}

//Send data to Raspberry though serial
//Using the already defined PureData protocol
void Looper::sendDataToPi(Channel msgChannel, uint8_t btnId){
    _serial->print((uint8_t)msgChannel);
    _serial->print(" ");
    _serial->print(btnId);
    _serial->println("");
}


//Get data to Raspberry though serial
//Using the already defined PureData protocol
void Looper::getDataFromPi(){
    uint8_t buffer[3], i=0;
    if(_serial->available() >= 3){
        while(_serial->available() > 0){
            buffer[i] = (_serial->read() - '0');
            i++;
        }
        updateTrackState(buffer);
    }
}

/*  Update loop track state using data received drom PureData.
    If msg[0] == 0:STATUS
    msg[1]: 0:CLEAR_REC  1: START_REC    2:STOP_REC  3:START_OVERDUB
            4:STOP_OVERDUB  5:WAIT_REC  6:MUTE_REC
    msg[2]: loop track nummber (1-8)

    If msg[0] == 1:COUNTER
    msg[1]: actual metronome count
    msg[2]: metronome max value
*/
void Looper::updateTrackState(uint8_t *msg){
    uint8_t trackNumber = msg[2] - 1;
    TrackState newState = static_cast<TrackState>(msg[1]);
    if(msg[0] == STATUS){
        if(_trackState[trackNumber] == MUTE_REC && newState == MUTE_REC)                            // If already mute then unmute
            _trackState[trackNumber] = STOP_REC;                                                    // Unmute
        else
            _trackState[trackNumber] = newState;
        debugLoopTrack(trackNumber);
    }
    else if(msg[0] == COUNTER){
        _metronome = msg[1];
        _metronomeMax = msg[2];
    }
    
}


//Update drumpad buttons
void Looper::updateDrumpad(){
    // Update drumPad
    if (_drumpad->getKeys()){
        for (uint8_t i=0; i< (_drumpad->getNumberKeys()) ; i++){            // Scan the whole key list.
            Key k = _drumpad->keys[i];
            if ( k.stateChanged && k.state != RELEASED){                    // Only find keys that have changed state.
                sendDataToPi(BTN_PRESSED, k.id);                            // Send data to Pi. Third byte is not used.
           }
        }
    }
}


// Update trackpad buttons. 
void Looper::updateTrackpad(){
    // Update trackpad
    if (_trackpad->getKeys()){
        for (uint8_t i=0; i< (_trackpad->getNumberKeys()) ; i++){                           // Scan the whole key list.
            Key k = _trackpad->keys[i];           
            if (k.stateChanged && k.state != RELEASED){                                     // Only find keys that have changed state.
                Channel msgCh = BTN_PRESSED;                                                //Start-stop rec
                if (k.state == HOLD){
                    msgCh = CLEAR_LOOP;                                                     // Clear
                }                                 
                else if(_trackState[i] == STOP_REC && k.state == PRESSED){
                    if(_muteKey.state == PRESSED || true )  msgCh = BTN_PRESSED;            // ISSUE: Mute 
                    else                                    msgCh = OVERDUB;                // Overdub 
                }              
                sendDataToPi(msgCh, k.id);                                                  // Send data to Pi
           }
        }
    }
}


void Looper::debugKey(Key k){
    String msg = "IDLE";
    Serial.print("Button ID: "); Serial.print( k.id);
    Serial.print(" state: "); 
    switch(k.state){
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
    Serial.print(" state changed: "); Serial.print( k.stateChanged);
    Serial.print(" actual value: "); Serial.print( k.actualValue);
    Serial.println("");
}

void Looper::debugLoopTrack(uint8_t id){
    String msg = "";
    switch(_trackState[id]){
        case CLEAR_REC:         msg = "CLEAR_REC";  break;
        case START_REC:         msg = "START_REC"; break;
        case STOP_REC:          msg = "STOP_REC"; break;
        case START_OVERDUB:     msg = "START_OVERDUB"; break;
        case STOP_OVERDUB:      msg = "STOP_OVERDUB"; break;
        case WAIT_REC:          msg = "WAIT_REC"; break;
        case MUTE_REC:          msg = "MUTE_REC"; break;
    }
    Serial.print(" TRACK ID "); Serial.print(id+1);
    Serial.print(" STATUS "); Serial.print(msg);
    Serial.println("");

}