#include "Keypad.h"

#define LOOP_TRACKS 8                           //Define nmber of loop tracks
typedef enum {EMPTY, REC, PLAY, OVERDUB, MUTE}Track;

class Looper{
    private:
        Keypad* _drumpad;
        Keypad* _trackpad;
        HardwareSerial* _serial;
        double _baudRate;
        Track _tracks[8];
        Key _muteKey;                //ISSUE: Not connected
        
    public:
        Looper(Keypad* drumpad, Keypad* trackpad, HardwareSerial* s, double baudRate);
        void init();
        void sendDataToPi(byte msg, byte value);
        void updateDrumpad();
        void updateTrackpad();
        void debugKey(Key k);
};



