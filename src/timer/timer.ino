// https://github.com/ThePcCreator/Arduino-4-Digit-7-Segment-Display-Library?tab=readme-ov-file
// https://github.com/ThePcCreator/Arduino-4-Digit-7-Segment-Display-Library/releases/tag/v1.1


#include "Display_Controller_New.h"
#include "Wire.h"

int timeRemainingStart = 30;
int timeRemaining = 30;
bool winner = false;


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
      writeNum(timeRemaining, 3000);
      return;
   }

   delay(750);
   Serial.println(timeRemaining);
   Wire.beginTransmission(9);
   Wire.write(timeRemaining);
   Wire.endTransmission();
/*
   if (winner) {
      for (int i=0; i<5; i++) {
         writeNum(timeRemaining, 1000);
         delay(500);
      }
      winner = false;
      timeRemaining = 30;
   }
*/
/*
   if (timeRemaining > 0) {
      writeNum(timeRemaining--, 3000);
      //delay(500);
   } else {
      // 1. Reset counter
      //timeRemaining = timeRemainingStart;

      // 2. Just continually write 0
      timeRemaining = 0;
      writeNum(0, 3000);

      // 3. Flash 8888
      //writeNum(8888, 1000);
      //delay(500);
   }
*/
   Serial.println("requesting data");
   Wire.requestFrom(9, 1); // Request 6 bytes from slave address 9
   while (Wire.available()) { // While data is available
      char c = Wire.read(); // Read a byte
      Serial.print(c); // Print it to serial monitor
   
      if (c == '0') {
         Serial.println("no winner yet");
         timeRemaining = 30;
         writeNum(timeRemaining, 3000);
         winner = false;
      } else if (c == '1' ||  c == '2' || c == '3' || c == '4') {
         // do nothing
         writeNum(timeRemaining--, 3000);
      } else if (c == '5') {
         Serial.println("winner");
         winner = true;
      } else {
         writeNum(timeRemaining, 3000);
      }
      break;
   }
   Serial.println(); // New line after receiving data

}
