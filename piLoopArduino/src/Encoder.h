#include <Arduino.h>


class Encoder{
    private:
        uint8_t _pinCLK, _pinDT, _pinSW, _pToSelect;
        uint8_t _currentStateCLK, _lastStateCLK, _dir;
        bool _pressed;
        long _count, _ppr;


    public:
        Encoder(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW, long ppr, uint8_t pToSelect);
        void init();
        void updateEncoder();
        uint8_t getPulses();
        bool isPressed();
};