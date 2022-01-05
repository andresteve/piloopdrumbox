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
  _tft->setFont(Arial_14);

  _menuEncoder->init();
  _menuState = MAIN_MENU;
  _nMenuItems = sizeof(_mainMenu) / sizeof(_mainMenu[0]);
  _menuPtr = _mainMenu;
  drawMenu();
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
  _tft->setTextColor(ILI9341_SLATEGRAY);
  _tft->printAt("BPM:",180,30);  
  _tft->setTextColor(ILI9341_WHITE);
  _tft->printAt(newBpm,230,30);  
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
  uint8_t newPos = 2*p;
  if(newPos==0){                    // Reset all
    newPos = nRect;
    color = ILI9341_BLACK;
  }
  for(uint8_t i = 0; i<= newPos; i++){
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
            inColor = ILI9341_WHITE;
            break;
        case START_OVERDUB:
            inColor = ILI9341_ORANGE;
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
  if(t.state == STOP_REC){                                     
    uint16_t x0 =  circleX -5; uint16_t y0 = circleY +5;
    uint16_t x1 =  circleX +5; uint16_t y1 = circleY;
    uint16_t x2 =  circleX -5; uint16_t y2 = circleY -5;
    _tft->fillTriangle(x0, y0, x1, y1, x2, y2, ILI9341_GREEN); // Play symbol
  }
  else if (t.state == MUTE_REC){
    uint16_t x0 =  circleX - w/2 + 10 ; uint16_t y0 = circleY + h/2 -10;
    uint16_t x1 =  circleX + w/2 -10;  uint16_t y1 = circleY - h/2 + 10;
    _tft->drawLine(x0, y0, x1, y1, ILI9341_GRAY);
  }
  else if(t.state == START_REC){
    _tft->fillCircle(circleX, circleY, 5, ILI9341_RED);
  }
  else if(t.state == START_OVERDUB){
    _tft->fillCircle(circleX, circleY, 5, ILI9341_ORANGE);
  }
  
}


/**
 * @brief Update TFT screen and menu encoder.
 *        Must be called in the main loop. 
 * 
 */
void TFT::update(){
    _menuEncoder->updateEncoder();
    updateMenu();
}

void TFT::updateMenu(){
  // Draw the index idxow
  if (_menuEncoder->isMoving()){   
    if(_menuEncoder->getDirection() == CW)       _selectedItem ++;               // Go down
    else                                         _selectedItem --;               // Go up
    if(_selectedItem < 0)                        _selectedItem = _nMenuItems -1;
    else if(_selectedItem >= _nMenuItems)        _selectedItem = 0;              // Circular motion
    drawMenu();
    _exitMenuTimer = millis();
  }
  switch (_menuState){
    case MAIN_MENU:
      if (_menuEncoder->isPressed()) {
        if (_selectedItem == 0)       _menuState = SOUND_MENU;
        else if (_selectedItem == 1)  _menuState = FX_MENU;
        else if (_selectedItem == 2)  _menuState = EXIT;
        _selectedItem = 0;
        _nMenuItems = sizeof(_soundMenu) / sizeof(_soundMenu[0]);
        _menuPtr = _soundMenu;
        _exitMenuTimer = millis();
      }
      break;

    case SOUND_MENU:
      if (_menuEncoder->isPressed())    _menuState = LOAD_SOUND;
      else if(exitMenuForTimeout(5000)) _menuState = EXIT;
      break;

    case LOAD_SOUND:
      _menuState = MAIN_MENU;
      _selectedItem = 0;
      _nMenuItems = sizeof(_mainMenu) / sizeof(_mainMenu[0]);
      _menuPtr = _mainMenu;
      break;

    case FX_MENU:
      break;

    case EXIT:
      _menuState = MAIN_MENU;
      _selectedItem = 0;
      _nMenuItems = sizeof(_mainMenu) / sizeof(_mainMenu[0]);
      _menuPtr = _mainMenu;
      break;
  }
  
  if(_menuState != _oldMenuState)    drawMenu();  // Force update
  _oldMenuState = _menuState;

}


void TFT::clearMenu(){
   uint8_t sX = 30, sY = 20;
   _tft->fillRect(_menuStartX-sX, _menuStartY-sY, _menuW+sX, _menuSpacingY * _maxItemToShow + sX, ILI9341_BLACK);
}

void TFT::drawMenu(){ 
  // Clear menu
  clearMenu();

  // Check scroll down  
  if(_selectedItem >= _maxItemToShow)   _scrollIndex = _selectedItem - (_maxItemToShow-1);                             
  else                                  _scrollIndex = 0;       

  // Draw the menu
  for (uint8_t i = _scrollIndex; i < _nMenuItems; i++){
    if(i < (_maxItemToShow+_scrollIndex)){
       _tft->setTextColor(ILI9341_WHITE);  
       if( i == _selectedItem){
          _tft->setTextColor(ILI9341_RED);  
       }      
      _tft->printAt(_menuPtr[i], _menuStartX, _menuStartY + (i-_scrollIndex) * _menuSpacingY);
    }
  }
  // Draw navbar
  drawNavBar();
}

void TFT::drawNavBar(){
    uint8_t width = 8, r = 3 , b = width, h = 8;
    uint8_t spacer = 3;
    uint8_t x0,y0,x1,y1,x2,y2;
    uint16_t color = ILI9341_GRAY;

    // Poistion relative to this first point
    uint8_t firstPointX = _menuStartX - width - spacer;     
    uint8_t firstPointY = _menuStartY - width/2;

    // Upper triangle
    if(_scrollIndex >=1){
      x0 = firstPointX;  y0 = firstPointY - spacer;
      x1 = x0 + b/2;     y1 = y0 - h;
      x2 = x0 +b;        y2 = y0;
      _tft->fillTriangle(x0,y0,x1,y1,x2,y2, color);   
    }
    
    // External navbar
    _tft->drawRoundRect(firstPointX, firstPointY, width, _maxItemToShow *_menuSpacingY, r, color); 

    //Inner navbar depending on selected items
    uint8_t navHeigth = (_maxItemToShow *_menuSpacingY) / _nMenuItems;
    uint8_t navPosY = firstPointY +_selectedItem * navHeigth - 1;
    _tft->fillRoundRect(firstPointX+1, navPosY, width-2, navHeigth, r, color);    

    // Down triangle
    if(_nMenuItems > _maxItemToShow && _selectedItem != (_nMenuItems-1)){
      x0 = firstPointX; y0 = firstPointY + _maxItemToShow *_menuSpacingY + spacer;
      x1 = x0 + b/2;    y1 = y0 + h;
      x2 = x0 +b;       y2 = y0;
      _tft->fillTriangle(x0,y0,x1,y1,x2,y2, color);   
    }
}

bool TFT::exitMenuForTimeout(unsigned long timeout){
  if ((millis() - _exitMenuTimer) > timeout)   return true;
  else                                        return false;
}

bool TFT::loadSound(){
  if(_menuState == LOAD_SOUND)  return true;
  else                          return false;
}

uint8_t TFT::getSelectedItem(){
  return _selectedItem;
}