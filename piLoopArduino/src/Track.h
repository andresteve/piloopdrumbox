#ifndef _TRACK_H_
#define _TRACK_H_

#include <Arduino.h>
#define VOLUME_THR 10

typedef enum {CLEAR_REC, START_REC, STOP_REC, START_OVERDUB, STOP_OVERDUB, WAIT_REC, MUTE_REC}  TrackState;

class Track{
    private:
        uint8_t _id, _analogIn, _volume;
        bool _volumeChanged, _muxIsUsed;
        uint8_t _muxS0, _muxS1, _muxS2;
        uint16_t _xStart, _yStart, _height, _width, _radius;     // Graphic dimension to display a rectangle on screen
        uint16_t _color;
    public:
        TrackState state;
        Track(uint8_t id, uint8_t analogIn, uint8_t muxS0, uint8_t muxS1, uint8_t muxS2, bool muxIsUsed);
        void setGraphics(uint16_t xStart,uint16_t yStart,uint16_t height, uint16_t width, uint16_t radius, uint16_t color);
        void init();
        bool update();
        void serialDebug();
        uint8_t getId(){return _id;};
        uint8_t getVolume(){ return _volume;};
        uint16_t getX(){ return _xStart;};
        uint16_t getY(){ return _yStart;};
        uint16_t getHeight(){ return _height;};
        uint16_t getWidth(){ return _width;};
        uint16_t getRadius(){ return _radius;};
        uint16_t getColor(){ return _color;}
};

#endif