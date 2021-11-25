#include "Keypad.h"

class Looper{
    private:
        Keypad* _keypad;
        HardwareSerial* _serial;
        double _baudRate;
    public:
        Looper(Keypad* k,  HardwareSerial* s, double baudRate);
        void init();
        void sendDataToPi(String msg, int value);
        void updateButtons();
};



