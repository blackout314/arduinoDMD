// =========================================================================================
// About: This a text scroller for 8x8 LED matrix (currently supports 2)
//        Will only work for text rotated 90 CCW, please arrange your matricies accordingly
// Author: Bratan (clickglide.com)
// Last modified: 2012/10/21 (V 1.0)
// Notes: Work in progress, character sizes are not consistent
// In this verions we adding full ASCII support with offset for 1st 32 chars
// Small issue, lengh of myText is not determined correctly (had to subtract 1) to get it working. Not sure why...
#include "LedControl.h"
#include <avr/pgmspace.h>
#include "font8x8.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 9 is connected to the DataIn
 pin 10 is connected to the CLK
 pin 11 is connected to LOAD
 Lastly specify number devices tonnected to MAX72XX.
 */

//LedControl lc=LedControl(9,10,11,2); // Replace with your pin numbers
const int numDevices = 3;      // number of MAX7219s used
LedControl lc=LedControl(10,12,11,numDevices);
// User configurable Variables
// ------------------------------------------------------------------------
char myText[]={"Two Matrix LED test 1234567890 "}; // replace with your text
const int animationDelay(100); // Adjusts scrolling speed
// ------------------------------------------------------------------------
byte charBuffer[8]; // Buffer for screen 1
byte charBuffer2[8]; // Buffer for screen 2
byte charBuffer3[8]; // Buffer for screen 2
int charIndex=0; //Remember position in the myText string
int wordLen=sizeof(myText)/sizeof(char)-1; // Determine number of elements (letters) in array
int firstRun=0; // For cosmetical purposes
byte nextChar;


void setup() {
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  for(int address=0;address<numDevices;address++) { // Initialize all devices in the chain
    lc.shutdown(address,false);
    lc.setIntensity(address,8);
    lc.clearDisplay(address);
  }
  //Serial.begin(9600);

}

// ==== Display Buffer on LED matrix ====
void bufDisplay(int LED, byte* buffer) {
 for(int i=0;i<8;i++) { // Render Character on Screen 1
     lc.setRow(LED,i,buffer[i]);
     }
     //rotateBuffer(myCount);
}


// ==== Fill Buffer with character data ====
void fillBuffer(){
   for (int i=0;i<8;i++){
    charBuffer[i]=pgm_read_byte(&font8x8[myText[charIndex]-32][i]); //Reference actual char from the font
  }
  //===============================================================================
  charIndex++; // Increment position in the myWord for next run
     if (charIndex>wordLen-1){ charIndex=0; } // if poistion one less than lengh, reset position to 0
}
// ------------------------------------------------------------------------------

// === Rotate buffer by swapping elements ====
void rotateBuffer(byte* buffer,int nextChar){

  for (int i=7;i>-1;i--){ // Rotate array
     if (i>0) { buffer[i]=buffer[i-1]; //  Swap elements 0-6
     }
     else { buffer[i]=nextChar; } // At pos 7 Add element from next character

  }
}

// ------------------------------------------------------------------------------

void loop() {
   fillBuffer(); //Let's fill buffer with first/next character in the word

   for (int j=7;j>-1;j--){ //Rotate 8 times to create animation of scrolling
     nextChar=pgm_read_byte(&font8x8[myText[charIndex]-32][j]); // Figure out what is the following character after current

    // This "First Run" part of the program is optional
    // -------------------------------------------------------------
     if (firstRun!=0) { // Shows character before it rotated.
       bufDisplay(0,charBuffer); // Display 1st buffer
       bufDisplay(1,charBuffer2); //Display Buffer 2
       bufDisplay(2,charBuffer3); //Display Buffer 2
     } //
     firstRun=1; // Set flag that it's not first run of the program
    // -------------------------------------------------------------

     rotateBuffer(charBuffer3,charBuffer2[7]); // Rotate/fill 2nd buffer (for second matrix) first
     rotateBuffer(charBuffer2,charBuffer[7]); // Rotate/fill 2nd buffer (for second matrix) first
     rotateBuffer(charBuffer,nextChar); // Rotate buffer for first matrix screen

     bufDisplay(0,charBuffer); // Show frame on matrix 1
     bufDisplay(1,charBuffer2); // Show frame on matrix 1
     bufDisplay(2,charBuffer3); // Show frame on matrix 1

     delay (animationDelay); // Delay before showing next frame
   }
}
