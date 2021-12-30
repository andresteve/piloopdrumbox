#include "Looper.h"
#include "Keypad.h"

/**
 * @brief Looper constructor
 * 
 * @param drumpad Keypad object to play sound
 * @param trackpad Keypad object to handle looptracks
 * @param loopTracks Track object array to handle loopTracks
 * @param tft TFT object to control tft screen and encoder 
 * @param leds CRGB object to control leds
 * @param muteKey Key object to mute loop tracks
 * @param s Serial used to communicate with Raspberry
 * @param baudRate Serial baud rate
 */
Looper:: Looper(Keypad* drumpad, Keypad* trackpad, Track* loopTracks, TFT* tft, CRGB* leds, Key * muteKey, HardwareSerial* s, double baudRate){
    _drumpad = drumpad;
    _trackpad = trackpad;
    _loopTracks = loopTracks;
    _loopTracksNumber = _trackpad->getNumbersRows() * _trackpad->getNumberColumns();
    _tftObj = tft;
    _leds = leds;
    _muteKey = muteKey;
    _serial = s;
    _baudRate = baudRate;
}

/**
 * @brief Initialize Looper object.
 *        Initialize serial communication.
 *        Initialize drumpad, trackpad, tft objects.
 *        Initialize loop tracks.
 * 
 */
void Looper::init(){
    _serial->begin(_baudRate);
    _drumpad->init();
    _trackpad->init();
    _muteKey->init(INPUT_PULLUP);
    _tftObj->init();

    // Assign graphic part to each track
    int colors[] = {ILI9341_BLUE, ILI9341_GREEN, ILI9341_YELLOW, ILI9341_RED};
    uint16_t xStart = 40, yStart = 120;
    uint8_t spacingX = 10, spacingY = 10;
    uint16_t h = 50, w =50, radius = 3;
    uint8_t id = 0;
    for(uint8_t r=0; r<_trackpad->getNumbersRows(); r++){
        uint16_t y = yStart + r * (h+spacingY);
        for(uint8_t c=0; c<_trackpad->getNumberColumns(); c++){
            uint16_t x = xStart + c * (w+spacingX);
            _loopTracks[id].setGraphics(x, y, h, w, radius, colors[c]);
            _loopTracks[id].init();
            _tftObj->drawLoopTrack( _loopTracks[id]);
            id++;
        }
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
 * @param btnId Pressed button's ID / Loop track
 * @param value Used to send potentiometers values (volume) 
 */
void Looper::sendDataToPi(Channel msgChannel, uint8_t btnId, uint8_t value){
    _serial->write((uint8_t)msgChannel);
    _serial->write(btnId);
    _serial->write(value);
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
            if( i== 3){
               updateTrackState(buffer); 
               i=0;
            }
        }
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
    TrackState newState = static_cast<TrackState>(msg[1]);  
    uint8_t trackNumber = msg[2] - 1;                                                                   // In puredata tracknumber goes from 1-8    
    if(msg[0] == STATUS){
        if(_loopTracks[trackNumber].state == MUTE_REC && newState == MUTE_REC)    newState = STOP_REC;   // If already mute then unmute
        _loopTracks[trackNumber].state = newState;                                                       
        changeTrackLedColor(trackNumber);
        _tftObj->drawLoopTrack(_loopTracks[trackNumber]);
    }
    else if(msg[0] == COUNTER){
        _bpmCount = msg[1];
        _bpm = msg[2];
        _tftObj->drawBpm((String)_bpm);
        _tftObj->drawPosition(_bpmCount);
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
                    sendDataToPi(BTN_PRESSED, k.id, 0);                     // Send data to Pi. 
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
 *        Update loop tracks volume.
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
                else if(_loopTracks[i].state == STOP_REC && k.state == PRESSED){
                    if(_muteKey->state == PRESSED || true )  msgCh = LOOP_PRESSED;           // ISSUE (cancel true): Mute 
                    else                                     msgCh = OVERDUB;                // Overdub 
                }              
                sendDataToPi(msgCh, i, 0);                                                   // Send data to Pi. Channel, id(0-7), value (not used)
           }
        }
    }
    // Update track volume
    bool volumeChanged = false;
    for(uint8_t i=0; i<_loopTracksNumber; i++){
       volumeChanged = _loopTracks[i].update();
       if(volumeChanged){
           sendDataToPi(VOLUME, i,  _loopTracks[i].getVolume());
           Serial.println(_loopTracks[i].getVolume());
       }
    }
}


void Looper::update(){
  updateDrumpad();
  //_muteKey->update(_muteKey->pin);
  updateTrackpad();
  _tftObj->update();
  
}

/**
 * @brief Update loop track's led color depending on its state.
 * 
 * @param trackNumber loop track number 
 */
void Looper::changeTrackLedColor(uint8_t trackNumber){
    CRGB c = CRGB::Black;
    switch (_loopTracks[trackNumber].state){
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
}



