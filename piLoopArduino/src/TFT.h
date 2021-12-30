#ifndef _TFT_H_
#define _TFT_H_

#include <Arduino.h>
#include "ILI9341_due.h"
#include "Encoder.h"
#include "Arial14.h"
#include "Track.h"

#define TFT_WIDTH   320
#define TFT_HEIGHT  240
typedef enum{PLAY, FILTER, SAVE}MenuState;

/**
 * @brief This class control a TFT screeen based on ILI9341 chip.
 *        It include an incremental encoder to handle a menu.
 * 
 */
class TFT{
    private:
        uint8_t _pinCS, _pinDC, _pinRST;
        ILI9341_due* _tft;
        Encoder *_menuEncoder;

    public:
        TFT(uint8_t CS, uint8_t DC, uint8_t RST, Encoder* enc);
        void init();
        void debugMessage(String pt1, String pt2, String pt3);
        void drawInstrument(uint8_t instNum);
        void update();
        void updateMenu();
        void drawBpm(String newBpm);
        void drawPosition(uint8_t p);
        unsigned long testText(); 
        unsigned long testLines(uint16_t color);
        void drawLoopTrack(Track t);
};

#endif