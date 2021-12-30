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
  _tft->fillScreen(ILI9341_BLACK);
  _menuEncoder->init();
  _tft->setFont(Arial_14);
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
 * @brief Draw loop bpm
 * 
 * @param newBpm 
 */
void TFT::drawBpm(String newBpm){
  _tft->fillRect(20, 30, 140, 30, ILI9341_BLACK);
  _tft->setTextColor(ILI9341_SLATEGRAY);
  _tft->printAt("BPM:",20,30);  
  _tft->setTextColor(ILI9341_WHITE);
  _tft->printAt(newBpm,70,30);  
}

/**
 * @brief Draw position of the loop
 * 
 * @param p
 */
void TFT::drawPosition(uint8_t p){
  uint8_t spacing = 5, nRect = 16;
  uint8_t startX = spacing, startY = 5;
  uint8_t rectW = (uint8_t) (TFT_WIDTH - (nRect*spacing)) / nRect, rectH = 20;
  int color = ILI9341_WHITE;
  if(p==0){                    // Reset all
    p = nRect;
    color = ILI9341_BLACK;
  }
  for(uint8_t i = 0; i<p; i++){
    _tft->fillRect(startX + i*(rectW+ spacing), startY, rectW, rectH, color);
  }
}

/**
 * @brief Draw loop tracks on the screen
 * 
 */
void TFT::drawLoopTrack(Track t){
  uint16_t inColor = ILI9341_BLACK;
  uint16_t x = t.getX();      uint16_t y = t.getY();
  uint16_t h = t.getHeight(); uint16_t w = t.getWidth();
  uint16_t r = t.getRadius(); uint16_t extColor = t.getColor();
  switch (t.state){
        case CLEAR_REC:
            inColor = ILI9341_BLACK;
            break;
        case START_REC:
            inColor = ILI9341_WHITE;
            break;
        case STOP_REC:
            inColor = ILI9341_SNOW;
            break;
        case START_OVERDUB:
            inColor = ILI9341_YELLOW;
            break;
        case STOP_OVERDUB:
            inColor = ILI9341_WHITE;
            break;
        case WAIT_REC:
            inColor = ILI9341_YELLOW;
            break;
        case MUTE_REC:
            inColor = ILI9341_GRAY;
            break;
    }
  _tft->drawRoundRect(x, y , h, w, r, extColor);      // External rect
  _tft->fillRoundRect(x+2, y+2 , h-4, w-4, r, inColor);   // Inner rect
  uint16_t circleX = x + w/2; 
  uint16_t circleY = y + h/2;
  _tft->fillCircle(circleX, circleY, 15, ILI9341_BLACK);       // Black circle in the middle
  if(t.state == STOP_REC){                                     // Play symbol
    uint16_t x0 =  circleX -5; uint16_t y0 = circleY +5;
    uint16_t x1 =  circleX +5; uint16_t y1 = circleY;
    uint16_t x2 =  circleX -5; uint16_t y2 = circleY -5;
    _tft->fillTriangle(x0, y0, x1, y1, x2, y2, ILI9341_WHITE);
  }
  else if (t.state == MUTE_REC){
    uint16_t x0 =  circleX - w/2 + 10 ; uint16_t y0 = circleY + h/2 -10;
    uint16_t x1 =  circleX + w/2 -10;  uint16_t y1 = circleY - h/2 + 10;
    _tft->drawLine(x0, y0, x1, y1, ILI9341_GRAY);
  }
  else if(t.state == START_REC){
    _tft->fillCircle(circleX, circleY, 5, ILI9341_RED);
  }
  
}


/**
 * @brief Update TFT screen and menu encoder.
 *        Must be called in the main loop. 
 * 
 */
void TFT::update(){
    _menuEncoder->getPulses();
    updateMenu();
}

void TFT::updateMenu(){
}

