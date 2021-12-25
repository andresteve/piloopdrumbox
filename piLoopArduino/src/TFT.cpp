#include "TFT.h"

/**
 * @brief Construct a new TFT object. 
 *        This class use ILI9341_due library to control the TFT screen.
 * 
 * @param CS SPI Chip Select. Can be any digital pin
 * @param DC Data / Command pin 
 * @param RST Reset pin
 * @param enc Encoder to control TFT menu
 */
TFT::TFT(uint8_t CS, uint8_t DC, uint8_t RST, Encoder* enc){
    _pinCS = CS;
    _pinDC = DC;
    _pinRST = RST;
    _menuEncoder = enc;
    _tft = new ILI9341_due(_pinCS, _pinDC, _pinRST);
}


/**
 * @brief Initialize TFT and menu encoder.
 * 
 */
void TFT::init(){
  _tft->begin();
	_tft->setRotation(iliRotation270);
  _menuEncoder->init();
}

/**
 * @brief Print a general message on TFT display
 * 
 * @param pt1 General string
 * @param pt2 General string 
 * @param pt3 General string
 */
void TFT::debugMessage(String pt1, String pt2, String pt3){
  _tft->fillRect(0, 100, 320, 240, 0x033f);
  _tft->setTextColor(ILI9341_WHITE);
  _tft->printAt(pt1,40,120);
  _tft->printAt(pt2,40,140);
  _tft->printAt(pt3,40,160);  
}


/**
 * @brief Draw instrument on display 
 * 
 * @param instNum: 0: Draw keys
 *                 1: Draw drums
 */
void TFT::drawInstrument(uint8_t instNum){
  _tft->fillScreen(ILI9341_BLACK);
  if(instNum == 0){
    //Draw Keys
    _tft->fillRect(50,70,220,160,ILI9341_AQUA);
    for(int i=0; i<7; i++){
      _tft->fillRect((i*30)+60,120,20,100,ILI9341_WHITE);
    }
    for(int i=0; i<6; i++){
      _tft->fillRect((i*30)+75,120,20,50,ILI9341_BLACK);
      if(i<4){
        _tft->fillCircle((i*30)+70,90,10,ILI9341_WHITE);
      }
    }
  }else if(instNum == 1){
    //Draw Drums
    _tft->fillRect(50,70,220,160,ILI9341_AQUA);
    for(int i=0; i<4; i++){
      _tft->fillRect((i*50)+60,130,40,40,ILI9341_WHITE);
    }
    for(int i=0; i<4; i++){
      _tft->fillRect((i*50)+60,180,40,40,ILI9341_WHITE);
    }
    for(int i=0; i<4; i++){
      _tft->fillCircle((i*30)+70,90,10,ILI9341_WHITE);
    }
    _tft->fillRect(180,80,80,20,ILI9341_WHITE);
  }
}


/**
 * @brief Update TFT screen and menu encoder.
 *        Must be called in the main loop. 
 * 
 */
void TFT::update(){
    _menuEncoder->getPulses();
}