#ifndef _ENCODER_H_
#define _ENCODER_H_


#include <Arduino.h>

/**
 * @brief This class control an incremental encoder.
 * 
 */
class Encoder{
    private:
        uint8_t _pinCLK, _pinDT, _pinSW, _pprDivider;
        uint8_t _currentStateCLK, _lastStateCLK, _dir;
        bool _pressed;
        long _count, _ppr;


    public:
        Encoder(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW, long ppr, uint8_t pprDivider);
        void init();
        void updateEncoder();
        uint8_t getPulses();
        uint8_t getDirection();
        bool isPressed();
};

#endif