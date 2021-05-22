/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */

// include TFT and SPI libraries
#include <TFT.h>  
#include <SPI.h>

// pin definition for Arduino UNO
#define cs   10
#define dc   9
#define rst  8

#define COLOR_FULL 255
#define COLOR_EMPTY 0 

int screen_width;
int screen_height;


// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
int x = 6;

void setup() {

  //initialize the library
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  //set the text size
  TFTscreen.setTextSize(2);

  screen_width = TFTscreen.width();
  screen_height = TFTscreen.height();


  // set a random font color
  TFTscreen.stroke(0, 0, 255);
}



void loop() {  
  // print Hello, World! in the middle of the screen
  TFTscreen.text("Hello, World!", x, 51 + x);

  TFTscreen.point(x, x);
  
  // wait 200 miliseconds until change to next color
  delay(20);
  x++;
  if (x > screen_height) {
    x = 0;
    TFTscreen.background(0, 0, 0);
  }
}
