#ifndef _TFT_H_
#define _TFT_H_

#include <Arduino.h>
#include "ILI9341_due.h"
#include "Encoder.h"
#include "Arial14.h"
#include "Track.h"

#define TFT_WIDTH   320
#define TFT_HEIGHT  240


typedef enum {MAIN_MENU, SOUND_MENU, LOAD_SOUND, FX_MENU, EXIT}MenuState;

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
        

        // Menu
        unsigned long _exitMenuTimer;
        int8_t _selectedItem;
        uint8_t _menuState, _oldMenuState, _nMenuItems;
        uint8_t _scrollIndex;
        const uint8_t _maxItemToShow = 3;
        const char* _mainMenu[3] = {"Load Sound", "Apply FX", "Quitta"}; 
        const char* _soundMenu[6] = {"Crash", "HH", "Kick", "Snare", "Piano", "Organ"}; 
        const char** _menuPtr;

        // Graphics
        const uint8_t _menuH = 20,  _menuW = 100;                   // Index backlight Heigth, Width dimensions
        const uint8_t _menuStartX = 40, _menuStartY = 50;
        const uint8_t _menuSpacingY = 20;

    public:
        TFT(uint8_t CS, uint8_t DC, uint8_t RST, Encoder* enc);
        void init();
        void drawInstrument(uint8_t instNum);
        void update();
        void updateMenu();
        void drawMenu();
        void drawNavBar();
        void clearMenu();
        void drawBpm(String newBpm);
        void drawPosition(uint8_t p);
        unsigned long testText(); 
        unsigned long testLines(uint16_t color);
        void drawLoopTrack(Track t);
        bool loadSound();
        bool exitMenuForTimeout(unsigned long timeout);
        uint8_t getSelectedItem();        
};

#endif