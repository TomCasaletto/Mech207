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
   Serial.println(timeRemaining);
   Wire.beginTransmission(9);
   Wire.write(timeRemaining);
   Wire.endTransmission();
   if (winner) {
      writeNum(timeRemaining, 1000);
      delay(500);
      return;
   }

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

   Serial.println("requesting data");
   Wire.requestFrom(9, 1); // Request 6 bytes from slave address 9
   while (Wire.available()) { // While data is available
      char c = Wire.read(); // Read a byte
      Serial.print(c); // Print it to serial monitor
   
      if (c == '0') {
         Serial.println("no winner yet");
      } else if (c == '1') {
         Serial.println("winner");
         winner = true;
      }
      break;
   }
   Serial.println(); // New line after receiving data

}
