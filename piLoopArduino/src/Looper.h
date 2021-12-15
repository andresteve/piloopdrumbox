#include "Keypad.h"

typedef enum {CLEAR_REC, START_REC, STOP_REC, START_OVERDUB, STOP_OVERDUB, WAIT_REC, MUTE_REC}  TrackState;
typedef enum {STATUS, COUNTER } MsgId;
typedef enum {AUDIO_MASTER, SELECT_KIT, BTN_PRESSED, CLEAR_LOOP, CLEAR_ALL, OVERDUB, AUDIO_INPUT}Channel;

class Looper{
    private:
        Keypad* _drumpad;
        Keypad* _trackpad;
        HardwareSerial* _serial;
        double _baudRate;
        TrackState _trackState[8];
        Key _muteKey;                //ISSUE: Not connected
        uint8_t _metronome;
        uint8_t _metronomeMax;
        
    public:
        Looper(Keypad* drumpad, Keypad* trackpad, HardwareSerial* s, double baudRate);
        void init();
        void sendDataToPi(Channel msgChannel, uint8_t btnId);
        void updateDrumpad();
        void updateTrackpad();
        void getDataFromPi();
        void updateTrackState( uint8_t *msg);
        void debugKey(Key k);
        void debugLoopTrack(uint8_t id);
};



