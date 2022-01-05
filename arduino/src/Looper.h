#ifndef _LOOPER_H_
#define _LOOPER_H

#include "Keypad.h"
#include "TFT.h"
#include "Track.h"
#include <FastLED.h>

typedef enum {STATUS, COUNTER } MsgId;
typedef enum {AUDIO_MASTER, DRUMPAD_SOUND, BTN_PRESSED, CLEAR_LOOP, CLEAR_ALL, OVERDUB, AUDIO_INPUT, LOOP_PRESSED, VOLUME}Channel;

/**
 * @brief This class control a looper station.
 *        It is composed by:
 *          - Drumpad to play sounds (Keypad object)
 *          - Trackpad to control loop tracks (Keypad object)
 *          - Mutekey to mute loop tracks (Key object)
 *          - TFT screen and encoder (TFT object)
 *          - Neopixel leds (CRGB object)
 */
class Looper{
    private:
        Keypad* _drumpad;
        Keypad* _trackpad;
        HardwareSerial* _serial;
        double _baudRate;
        Track* _loopTracks;
        Track* _loopMaster;
        Key* _muteKey;                
        uint8_t _bpmCount;
        uint8_t _bpm;
        CRGB* _leds;
        TFT* _tftObj;
        uint8_t _loopTracksNumber;
        
    public:
        Looper(Keypad* drumpad, Keypad* trackpad, Track* loopTracks, Track* loopMaster, TFT* tft, CRGB* leds, Key * muteKey, HardwareSerial* s, double baudRate);
        void init();
        void sendDataToPi(Channel msgChannel, uint8_t btnId, uint8_t value);
        void updateTrackState( uint8_t *msg);
        void update();
        void updateDrumpad();
        void updateTrackpad();
        void changeLedColor(uint8_t ledId, CRGB color);
        void changeTrackLedColor(uint8_t trackNumber);
        void getDataFromPi();

};

#endif


