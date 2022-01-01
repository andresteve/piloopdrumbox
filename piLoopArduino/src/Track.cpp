#include "Track.h"


Track::Track(uint8_t id, uint8_t analogIn, uint8_t muxS0, uint8_t muxS1, uint8_t muxS2, bool muxIsUsed){
    _id = id;
    _analogIn = analogIn;
    _muxS0 = muxS0;
    _muxS1 = muxS1;
    _muxS2 = muxS2;
    _muxIsUsed = muxIsUsed;
}


void Track::init(){
    pinMode(_analogIn, INPUT);
    if(_muxIsUsed){
        pinMode(_muxS0, OUTPUT);
        pinMode(_muxS1, OUTPUT);
        pinMode(_muxS2, OUTPUT);
    }
    state = CLEAR_REC;
    _volume = 0;
}


bool Track::update(){
    if(_muxIsUsed){
        uint32_t s2 = LOW, s1= LOW, s0= LOW;
        switch(_id){
            case 0: s2 = LOW,   s1= LOW,    s0= LOW;    break;
            case 1: s2 = LOW,   s1= LOW,    s0= HIGH;   break;
            case 2: s2 = LOW,   s1= HIGH,   s0= LOW;    break;
            case 3: s2 = LOW,   s1= HIGH,   s0= HIGH;   break;
            case 4: s2 = HIGH,  s1= LOW,    s0= LOW;    break;
            case 5: s2 = HIGH,  s1= LOW,    s0= HIGH;   break;
            case 6: s2 = HIGH,  s1= HIGH,   s0= LOW;    break;
            case 7: s2 = HIGH,  s1= HIGH,   s0= HIGH;   break;
        }
        digitalWrite(_muxS0, s0);
        digitalWrite(_muxS1, s1);
        digitalWrite(_muxS2, s2);
    }
    uint8_t actualVolume = map(analogRead(_analogIn),0, 1023, 0, 255);
    if((actualVolume <= (_volume + VOLUME_THR)) && (actualVolume >= (_volume - VOLUME_THR)))  _volumeChanged = false; // Significative change
    else  {
        _volumeChanged = true;
        _volume = actualVolume;
    }
    return _volumeChanged;
}

void Track::setGraphics(uint16_t xStart,uint16_t yStart,uint16_t height, uint16_t width, uint16_t radius, uint16_t color){
    _xStart = xStart;
    _yStart = yStart;
    _height = height;
    _width = width;
    _radius = radius;
    _color = color;
}


void Track::serialDebug(){
    String msg = "";
    switch(state){
        case CLEAR_REC:         msg = "CLEAR_REC";  break;
        case START_REC:         msg = "START_REC"; break;
        case STOP_REC:          msg = "STOP_REC"; break;
        case START_OVERDUB:     msg = "START_OVERDUB"; break;
        case STOP_OVERDUB:      msg = "STOP_OVERDUB"; break;
        case WAIT_REC:          msg = "WAIT_REC"; break;
        case MUTE_REC:          msg = "MUTE_REC"; break;
    }
    Serial.print(" TRACK ID "); Serial.print(_id);
    Serial.print(" STATUS "); Serial.print(msg);
    Serial.println("");

}