// https://github.com/ThePcCreator/Arduino-4-Digit-7-Segment-Display-Library?tab=readme-ov-file
// https://github.com/ThePcCreator/Arduino-4-Digit-7-Segment-Display-Library/releases/tag/v1.1


#include "Display_Controller_New.h"
#include "Wire.h"

void(* resetFunc) (void) = 0; // reset

int timeRemainingStart = 30;
int timeRemaining = 30;
bool winner = false;
bool inRoom0 = true;

void setup() {
   Serial.begin(9600);
   Serial.println("-------------start------------");

   Wire.begin();

   // initialize  the digital pins as outputs.
   exeSetup();
   timeRemaining = timeRemainingStart;
 }

void loop() {
   if (winner || timeRemaining == 0) {
      Serial.println("resetting in 5 sec");
      // May only need this if not hooked up to motors
      for (int i=0; i<10; i++) {
         writeNum(timeRemaining, 3000);
         Wire.beginTransmission(9);
         Wire.write(timeRemaining);
         Wire.endTransmission();
         delay(500);
      }
      resetFunc();
      
      //return;
   }
   if (inRoom0) {
      // do nothing
   } else {
      timeRemaining--;
   }
   writeNum(timeRemaining, 3000);

   delay(750);
   Serial.println(timeRemaining);
   Wire.beginTransmission(9);
   Wire.write(timeRemaining);
   Wire.endTransmission();

   Serial.println("Requesting data from GameController");
   Wire.requestFrom(9, 1); // Request 1 byte from slave address 9
   while (Wire.available()) { // While data is available
      char c = Wire.read(); // Read a byte
      Serial.print(c);
      if (c == '0') {
         Serial.println("no winner yet");
      } else if (c == '1' ||  c == '2' || c == '3' || c == '4') {
         inRoom0 = false;
      } else if (c == '5') {
         Serial.println("winner");
         winner = true;
      } else {
         writeNum(timeRemaining, 3000);
      }
      break;

   }
}
