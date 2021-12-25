#include "Looper.h"
#include "Keypad.h"

/**
 * @brief Looper constructor
 * 
 * @param drumpad Keypad object to play sound
 * @param trackpad Keypad object to handle looptracks
 * @param tft TFT object to control tft screen and encoder 
 * @param leds CRGB object to control leds
 * @param muteKey Key object to mute loop tracks
 * @param s Serial used to communicate with Raspberry
 * @param baudRate Serial baud rate
 */
Looper:: Looper(Keypad* drumpad, Keypad* trackpad, TFT* tft, CRGB* leds, Key * muteKey, HardwareSerial* s, double baudRate){
    _drumpad = drumpad;
    _trackpad = trackpad;
    _tft = tft;
    _leds = leds;
    _muteKey = muteKey;
    _serial = s;
    _baudRate = baudRate;
}

/**
 * @brief Initialize Looper object.
 *        Initialize serial communication.
 *        Initialize drumpad, trackpad, tft objects.
 *        Clear loop tracks.
 * 
 */
void Looper::init(){
    _serial->begin(_baudRate);
    _drumpad->init();
    _trackpad->init();
    _tft->init();
    for(uint8_t i=0; i<_trackpad->getNumberKeys(); i++){
        _trackState[i] = CLEAR_REC;
    }
}

/**
 * @brief Send data to Raspberry though serial
 * 
 * @param msgChannel 0: AUDIO_MASTER
 *                   1: SELECT_KIT
 *                   2: BTN_PRESSED
 *                   3: CLEAR_LOOP
 *                   4: CLEAR_ALL
 *                   5: OVERDUB
 *                   6: AUDIO_INPUT
 *                   7: LOOP_PRESSED
 * @param btnId Pressed button's ID
 */
void Looper::sendDataToPi(Channel msgChannel, uint8_t btnId){
    _serial->print((uint8_t)msgChannel);
    _serial->print(" ");
    _serial->print(btnId);
    _serial->println("");
}



/**
 * @brief Get data from Raspberry. Use 3 byte message.
 * 
 */
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

/*  
    
*/

/**
 * @brief Update loop track state using data received from Raspberry.
 * 
 * @param msg The message is 3 byte long.
 *             If msg[0] == 0:STATUS
 *              msg[1]: 0:CLEAR_REC  1: START_REC    2:STOP_REC  3:START_OVERDUB  4:STOP_OVERDUB  5:WAIT_REC  6:MUTE_REC
 *              msg[2]: loop track nummber (1-8)
 *
 *             If msg[0] == 1:COUNTER
 *              msg[1]: actual metronome count
 *              msg[2]: metronome max value
 */
void Looper::updateTrackState(uint8_t *msg){
    uint8_t trackNumber = msg[2] - 1;                                                               // In puredata tracknumber goes from 1-8
    TrackState newState = static_cast<TrackState>(msg[1]);      
    if(msg[0] == STATUS){
        if(_trackState[trackNumber] == MUTE_REC && newState == MUTE_REC)    newState = STOP_REC;   // If already mute then unmute
        _trackState[trackNumber] = newState;                                                       
        changeTrackLedColor(trackNumber);
    }
    else if(msg[0] == COUNTER){
        _metronome = msg[1];
        _metronomeMax = msg[2];
    }  
}


/**
 * @brief Update drumpad buttons.
 *        Get pressed buttons and send a message to Raspberry if pressed.
 *        Change button's led color.
 */
void Looper::updateDrumpad(){
    // Update drumPad
    Key k; CRGB ledColor;
    if (_drumpad->getKeys()){
        for (uint8_t i=0; i< (_drumpad->getNumberKeys()) ; i++){            // Scan the whole key list.
            k = _drumpad->keys[i];
            if ( k.stateChanged){                                           // Only find keys that have changed state.
                if(k.state != RELEASED){
                    sendDataToPi(BTN_PRESSED, k.id);                        // Send data to Pi. 
                    ledColor = CRGB::Cyan;
                }
                else{
                    ledColor = CRGB::Black;
                }
                changeLedColor(k.idLed, ledColor);
           }
        }
    }
}


/**
 * @brief Update trackpad buttons.
 *        Get pressed buttons and send a message to Raspberry if pressed.
 *        Update loop tracks status.
 */
void Looper::updateTrackpad(){
    Key k; Channel msgCh;
    if (_trackpad->getKeys()){
        for (uint8_t i=0; i< (_trackpad->getNumberKeys()) ; i++){                           // Scan the whole key list.
            k = _trackpad->keys[i];           
            if (k.stateChanged && k.state != RELEASED){                                     // Only find keys that have changed state.
                msgCh = LOOP_PRESSED;                                                       // Start-stop rec
                if (k.state == HOLD){
                    msgCh = CLEAR_LOOP;                                                     // Clear
                }                                 
                else if(_trackState[i] == STOP_REC && k.state == PRESSED){
                    if(_muteKey->state == PRESSED || true )  msgCh = LOOP_PRESSED;           // ISSUE (cancel true): Mute 
                    else                                     msgCh = OVERDUB;                // Overdub 
                }              
                sendDataToPi(msgCh, i);                                                      // Send data to Pi. Channel + id(0-7)
           }
        }
    }
}

/**
 * @brief Update TFT screen
 * 
 */
void Looper::updateTFT(){
    _tft->update();
}

/**
 * @brief Update loop track's led color depending on its state.
 * 
 * @param trackNumber loop track number 
 */
void Looper::changeTrackLedColor(uint8_t trackNumber){
    CRGB c = CRGB::Black;
    switch (_trackState[trackNumber]){
        case CLEAR_REC:
            c = CRGB::Black;
            break;
        case START_REC:
            c = CRGB::Red;
            break;
        case STOP_REC:
            c = CRGB::Green;
            break;
        case START_OVERDUB:
            c = CRGB::Red;
            break;
        case STOP_OVERDUB:
            c = CRGB::GreenYellow;
            break;
        case WAIT_REC:
            c = CRGB::Yellow;
            break;
        case MUTE_REC:
            c = CRGB::Cyan;
            break;
    }
    changeLedColor(trackNumber, c);
}

/**
 * @brief Change neopixel led color
 * 
 * @param ledId led ID (position in the strip)
 * @param color CRGB color
 */
void Looper::changeLedColor(uint8_t ledId, CRGB color){
    _leds[ledId] = color;
    FastLED.show();
    Serial.print(" LED ");Serial.println(ledId);
}

/**
 * @brief Serial debug. Key status
 * 
 * @param k Selected key
 */
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

/**
 * @brief Serial debug. Loop track
 * 
 * @param id Loop track ID
 */
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