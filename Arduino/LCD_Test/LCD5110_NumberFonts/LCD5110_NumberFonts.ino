// LCD5110_NumberFonts 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of the included number-fonts,
// and how to use them.
//
// This program requires a Nokia 5110 LCD module.
//
// It is assumed that the LCD module is connected to
// the following pins using a levelshifter to get the
// correct voltage to the module.
//      SCK  - Pin 47
//      MOSI - Pin 46
//      DC   - Pin 45
//      RST  - Pin 43
//      CS   - Pin 44
//

#include <LCD5110_Basic.h>
#define BACKLIGHT 42
#define SCREEN_WAKE 53

LCD5110 myGLCD(47,46,45,43,44);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void setup()
{
  myGLCD.InitLCD();
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(SCREEN_WAKE, INPUT_PULLUP);
  digitalWrite(BACKLIGHT, 1);
}

void loop()
{
  
  if (digitalRead(SCREEN_WAKE) == LOW) {
    //Wake Screen & turn on backlight & update information
    digitalWrite(BACKLIGHT, 0);
    myGLCD.disableSleep();
    //myGLCD.setFont(SmallFont);
    //myGLCD.print("Hello World", CENTER, 16);
  }
  else {
    digitalWrite(BACKLIGHT, 1);
    myGLCD.enableSleep();
  }

  myGLCD.setFont(SmallFont);
  myGLCD.print("Hello World", CENTER, 16);
 
}
