#include <stdlib.h> 

int motor1Pos = 0;
int motor2Pos = 2000;
int command = 0;

void setup() {
  Serial.begin(9600); // Initialize hardware serial at 9600 baud
}

void resetGame()
{
  motor1Pos = 0;
  motor2Pos = 2000;
}

void loop() {
    if (Serial.available()) { // Check if data is available to read
      String receivedData = Serial.readStringUntil('\n'); // Read until a newline character
      command = strtol(receivedData.c_str(), NULL, 10);

      if (command == 0) {
        resetGame();
      } else if (command == 1) {
        //Serial.print("...send motor 1 position");
        Serial.println(motor1Pos++);
        if (motor1Pos > 8) {
          motor1Pos = 0;
        }
      } else if (command == 2) {
        //Serial.print("...send motor 2 position");
        Serial.println(motor2Pos++);
        if (motor2Pos > 2008) {
          motor2Pos = 2000;
        }
      } else if (command == 3) {
        // move to Room 2, stair 1
      } else if (command == 4) {
        // move to Room 2, stair 2
      } else if (command == 5) {
        // move to Room 2, stair 3
      } else if (command == 6) {
        // move to Room 2, stair 4
      } else if (command == 7) {
        // move to Room 2, stair 5
      } else if (command == 8) {
        // move to Room 3
      } else if (command == 9) {
        // spin player (indicate win)
      } else {
         // Unknown command, do nothing
      }
    }
}
