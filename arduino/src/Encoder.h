#ifndef _ENCODER_H_
#define _ENCODER_H_


#include <Arduino.h>

typedef enum {IDLE, CW, CCW}EncoderState;



/**
 * @brief This class control an incremental encoder.
 * 
 */
class Encoder{
    private:
        uint8_t _pinCLK, _pinDT, _pinSW, _pprDivider;
        uint8_t _currentStateCLK, _lastStateCLK, _dir;
        uint8_t _currentStateSW, _lastStateSW;
        bool _pressed;
        long _count, _ppr;


    public:
        Encoder(uint8_t pinCLK, uint8_t pinDT, uint8_t pinSW, long ppr, uint8_t pprDivider);
        void init();
        void updateEncoder();
        void debug();
        uint8_t getPulses();
        uint8_t getDirection();
        bool isPressed();
        bool isMoving();
};

#endif