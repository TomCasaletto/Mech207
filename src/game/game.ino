#include <IRremote.hpp>
#include "IRremote.h"
#include "Wire.h"


// Interrupt variables
volatile bool gameLevelToggle = false;
int gameLevelTogglePin = 2;
int timeRemaining = 60; // Read this from other board
bool useOtherBoard = true; // For timer testing (remove)

// Global variables
const int numRooms = 5; // Make room 0 be 'pre-coin drop' and room 4 the 'finish line'
bool roomCompleted[numRooms] = {0,0,0,0,0};
int currentRoom = 1;
bool timedOut = false;
unsigned long gameStartTime = 0;
unsigned long timeOutSec = 40;

// IR receiver variables (used for testing)
int receiver = 48;           // Signal Pin of IR receiver to Arduino Digital Pin
IRrecv irrecv(receiver);     // create instance of 'irrecv'
uint32_t last_decodedRawData = 0;

// Memory game parameters
int led1 = 12;
int led2 = 11;
int led3 = 10;
int led4 = 9;
int led5 = 8;
int roomTwoLeds[5] = {led1, led2, led3, led4, led5};
int gameLevel = LOW; // LOW=easy, HIGH=hard
float memoryFlashTimeSec = 0.75;
float memoryWaitTimeSec = 4;
int roomTwoLoopIdx = 0;

int buttonPinsMemoryGame[5] = {3,4,5,6,7};
int lastFiveMemoryGame[5] = {0,0,0,0,0};

// TBD: make these local variables?
const int numColorsMemory = 5;
int roomTwoAnswer[numColorsMemory] = {1,2,3,4,5};
uint32_t userInput5[numColorsMemory] = {0,0,0,0,0};
int answerIdx=0;

// Stair stepper game parameters
int led6 = 38;
int led7 = 40;
int led8 = 42;
int led9 = 44;
int led10 = 46;
int roomThreeLeds[5] = {led6, led7, led8, led9, led10};
int numStairsToJump = 3;

// Rune game parameters
int led11 = 31;
int led12 = 33;
int led13 = 35;
int roomFourLeds[3] = {led11, led12, led13};
float runeFlashTimeSec = 0.25;
int numRunesToDetect = 3;



void setup() {
   pinMode(buttonPinsMemoryGame[0], INPUT);
   pinMode(buttonPinsMemoryGame[1], INPUT);
   pinMode(buttonPinsMemoryGame[2], INPUT);
   pinMode(buttonPinsMemoryGame[3], INPUT);
   pinMode(buttonPinsMemoryGame[4], INPUT);

   pinMode(led1, OUTPUT);
   pinMode(led2, OUTPUT);
   pinMode(led3, OUTPUT);
   pinMode(led4, OUTPUT);
   pinMode(led5, OUTPUT);

   pinMode(led6, OUTPUT);
   pinMode(led7, OUTPUT);
   pinMode(led8, OUTPUT);
   pinMode(led9, OUTPUT);
   pinMode(led10, OUTPUT);

   pinMode(led11, OUTPUT);
   pinMode(led12, OUTPUT);
   pinMode(led13, OUTPUT);

   pinMode(gameLevelTogglePin, INPUT_PULLUP);

   Wire.begin(9);
   Wire.onReceive(receiveTimerEvent);

   irrecv.enableIRIn(); // Start the IR receiver
   randomSeed(analogRead(A0)); // Reading empty pin gives new seed every time
   Serial.begin(9600);
   Serial.println("------------------------------");
   Serial.println("------------NEW GAME----------");
   Serial.println("------------------------------");

   Serial1.begin(9600);

   setLevelSettings();

   char str[20];
   itoa(gameLevel, str, 10);
   Serial.println(strcat("  Game level=", str));

   setLowAll(2);
   setLowAll(3);

   attachInterrupt(digitalPinToInterrupt(gameLevelTogglePin), changeGameLevel, RISING); //LOW, CHANGE, RISING, FALLING
}

void loop() {
   Serial.println("-----looping--------");


   // Check for winner
   if (roomCompleted[numRooms-1]) {
      Serial.println("-----Game winner--------");
      // Announce winner
      // Save time
      // Reset game to start
   }

   if (timedOut) {
      Serial.println("-----Game loser TIMEOUT--------");
      // Announce loser
      // Reset game to start
      currentRoom = 0;
   }

   if (currentRoom==1) {
      // Check for coin in tight loop (assume this passes for now)
      Serial.println("-----In room 1--------");
      currentRoom = 2;
      gameStartTime = millis();

   } else if (currentRoom==2) {
      Serial.println("-----In room 2--------");
      doRoom2();
   } else if (currentRoom==3) {
      Serial.println("-----In room 3--------");
      doRoom3();
   } else if (currentRoom==4) {
      Serial.println("-----In room 4--------");
      doRoom4();
   } else if (currentRoom==5) {
      Serial.println("-----In room 5--------");
      // Flash lights to indicate end of game
      for (int i=0;i<5; i++) {
         setHighAll(5);
         delay(100);
         setLowAll(5);
         delay(100);
      }

      int finalCounter=1;
      while(1) {
         Serial.println("looping final until reset...");
         Serial.println(finalCounter++);
         delay(1000);
      }
   }
   delay(1000);
}

void changeGameLevel() {
   if (digitalRead(gameLevelTogglePin)==LOW) {
      Serial.println("-----TOGGLE is low,  setting gameLevel to EASY--------");
      gameLevel=LOW;
  } else {
      Serial.println("-----TOGGLE is high, setting gameLevel to HARD--------");
      gameLevel=HIGH;
   }
   gameLevelToggle = !gameLevelToggle;
   setLevelSettings();
}

void setLevelSettings() {
   Serial.println("-----reset game level numbers--------");
   if (gameLevel==HIGH) {
      memoryFlashTimeSec = 0.25;
      memoryWaitTimeSec = 3;

      numStairsToJump = 5;

      runeFlashTimeSec = 0.1;
      numRunesToDetect = 3;
   } else {
      memoryFlashTimeSec = 1.0;
      memoryWaitTimeSec = 7;

      numStairsToJump = 3;

      runeFlashTimeSec = 0.25;
      numRunesToDetect = 3;
   }
}

void receiveTimerEvent(int howMany)      
{
  int x = Wire.read();
  char buffer[30];
  strcpy(buffer, " Time remaining="); // Reset buffer to an empty string
  char num[30];
  itoa(x, num, 10);
  strcat(buffer, num);
  timeRemaining = x;

  Serial.println(x);
  //if (useOtherBoard && timeRemaining==0) {
  if (timeRemaining==-1) {
    currentRoom=1;
    timeRemaining = 60;
    // Flash lights to indicate end of game
    for (int i=0;i<5; i++) {
       setHighAll(5);
       delay(100);
       setLowAll(5);
       delay(100);
    }
  }

  Serial.println(buffer);
}

void flashLed(int ledNumber, int delayMilliSec, int numTimes) {
   for (int i=0;i<numTimes; i++) {
      digitalWrite(ledNumber, HIGH);
      delay(delayMilliSec);
      digitalWrite(ledNumber, LOW);
      delay(delayMilliSec);
   }
}

void setHighAll(int room)
{
   if (room==2) {
      digitalWrite(led1, HIGH);    
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);    
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
   } else if (room==3) {
      digitalWrite(led6, HIGH);    
      digitalWrite(led7, HIGH);
      digitalWrite(led8, HIGH);    
      digitalWrite(led9, HIGH);
      digitalWrite(led10, HIGH);
   } else if (room==4) {
      digitalWrite(led11, HIGH);    
      digitalWrite(led12, HIGH);
      digitalWrite(led13, HIGH);    
   } else if (room==5) {
      digitalWrite(led1, HIGH);    
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);    
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);    
      digitalWrite(led7, HIGH);
      digitalWrite(led8, HIGH);    
      digitalWrite(led9, HIGH);
      digitalWrite(led10, HIGH);
      digitalWrite(led11, HIGH);    
      digitalWrite(led12, HIGH);
      digitalWrite(led13, HIGH);    
   }
}

void setLowAll(int room)
{
   if (room==2) {
      digitalWrite(led1, LOW);    
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);    
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
   } else if (room==3) {
      digitalWrite(led6, LOW);    
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);    
      digitalWrite(led9, LOW);
      digitalWrite(led10, LOW);
   } else if (room==4) {
      digitalWrite(led11, LOW);    
      digitalWrite(led12, LOW);
      digitalWrite(led13, LOW);    
   } else if (room==5) {
      digitalWrite(led1, LOW);    
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);    
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);    
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);    
      digitalWrite(led9, LOW);
      digitalWrite(led10, LOW);
      digitalWrite(led11, LOW);    
      digitalWrite(led12, LOW);
      digitalWrite(led13, LOW);    
   }
}

void printLastFiveMemoryGame()
{
  char buffer[20];
  strcpy(buffer, "Last five="); // Reset buffer to an empty string

  for (int i=0; i<5; i++) {
    char num[2];
    itoa(lastFiveMemoryGame[i], num, 10);
    strcat(buffer, num);
    strcat(buffer, " ");
   }
   Serial.println(buffer);
}

void doRoom2() {
   
   while (!roomCompleted[2]) {

      unsigned long currentTime = millis();
      float duration = (currentTime-gameStartTime)/1000;
      setLowAll(2);

      // Check answer
      bool foundAnswer = true;
      answerIdx = 0;
      for (int i=0;i<5; i++) {
         // NOTE: when switched from IR input to buttons, had to reverse order of comparison? Fix later.
         //if (userInput5[i] != roomTwoAnswer[i]) {
         if (lastFiveMemoryGame[4-i] != roomTwoAnswer[i]) {
            Serial.println("-----Wrong answer ROOM 2, at position--------");
            Serial.println("Time elapsed:");
            Serial.println(duration);
            foundAnswer=false;
            break;
         }
      }

      if (foundAnswer) {
         Serial.println("-----WINNER ROOM 2!!!--------");
         roomCompleted[2] = true;
         currentRoom = 3;
         flashLed(led1, 250, 5);
         break;
      } else {
         Serial.println("-----still waiting ROOM 2!!!--------");
         flashLed(led5, 250,5 );
      }

      // Flash lights to indicate start of memory game
      for (int i=0;i<5; i++) {
         setHighAll(2);
         delay(100);
         setLowAll(2);
         delay(100);
      }
   
      // Create correct pattern
      roomTwoAnswer[0] = random(1,6)-1;
      for (int i=1; i<5; i++) {
         int nextColor = random(1,6)-1;
         while (nextColor == roomTwoAnswer[i-1]) {
            nextColor = random(1,6)-1;
         }
         roomTwoAnswer[i] = nextColor;
      }

      Serial.println("-----Correct pattern--------");
      for (int i=0; i<5; i++) {
         Serial.println(roomTwoAnswer[i]);
      }

      // Flash the correct pattern for the user
      for (int i=0; i<5; i++) {
         digitalWrite(roomTwoLeds[roomTwoAnswer[i]], HIGH);
         delay(memoryFlashTimeSec*1000);
         digitalWrite(roomTwoLeds[roomTwoAnswer[i]], LOW);
      }

      Serial.println("Loop for user to enter answer: ");
      unsigned long startTime = millis();
      long deltaTime = millis() - startTime;
      while (deltaTime < memoryWaitTimeSec*1000) {
         deltaTime = millis() - startTime;

         for (int i=0; i<5; i++) {
            int value = digitalRead(buttonPinsMemoryGame[i]);
            delay(10);
            if(value == HIGH) {
               for (int j=4; j>0; j--) {
                  lastFiveMemoryGame[j] = lastFiveMemoryGame[j-1];
               }
               lastFiveMemoryGame[0]=i;

               //Serial.println("-----BUTTON PRESSED--------");
               //Serial.println(i);
               printLastFiveMemoryGame();

               delay(500);
            }
         }
      }

// IR input section
/*
      // Now go into a loop for a certain time waiting for user input
      resetInput();
      Serial.println("Wait to enter answer: ");
      unsigned long startTime = millis();
      long deltaTime = millis() - startTime;
      while (deltaTime < memoryWaitTimeSec*1000) {
         deltaTime = millis() - startTime;
         //Serial.println(deltaTime);

         if (irrecv.decode()) { // have we received an IR signal?
            translateIR();
            irrecv.resume();
         }
      }
      Serial.println("User input: ");
      printInput();
*/
   }
}

void resetInput()
{
  Serial.println("resetting input...");
  for (int i=0; i<5; i++) {
    userInput5[i] = 0;
  }
  answerIdx = 0;
}

void printInput()
{
  for (int i=0; i<5; i++) {
    Serial.print(userInput5[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

int getIrInput()
{
   uint32_t val = 0;
   if (irrecv.decode()) { // have we received an IR signal?
 
   last_decodedRawData = irrecv.decodedIRData.decodedRawData;
   //map the IR code to the remote key
   switch (last_decodedRawData)
   {
      case 0xBA45FF00: Serial.println("POWER"); resetInput(); break;
      case 0xB847FF00: Serial.println("FUNC/STOP"); break;
      case 0xB946FF00: Serial.println("VOL+"); break;
      case 0xBB44FF00: Serial.println("FAST BACK");    break;
      case 0xBF40FF00: Serial.println("PAUSE");    break;
      case 0xBC43FF00: Serial.println("FAST FORWARD");   break;
      case 0xF807FF00: Serial.println("DOWN");    break;
      case 0xEA15FF00: Serial.println("VOL-");    break;
      case 0xF609FF00: Serial.println("UP");    break;
      case 0xE619FF00: Serial.println("EQ");    break;
      case 0xF20DFF00: Serial.println("ST/REPT");    break;
      case 0xE916FF00: Serial.println("0");    val=0; break;
      case 0xF30CFF00: Serial.println("1");    val=1; break;
      case 0xE718FF00: Serial.println("2");    val=2; break;
      case 0xA15EFF00: Serial.println("3");    val=3; break;
      case 0xF708FF00: Serial.println("4");    val=4; break;
      case 0xE31CFF00: Serial.println("5");    val=5; break;
      case 0xA55AFF00: Serial.println("6");    val=6; break;
      case 0xBD42FF00: Serial.println("7");    val=7; break;
      case 0xAD52FF00: Serial.println("8");    val=8; break;
      case 0xB54AFF00: Serial.println("9");    val=9; break;
      default:
         Serial.println(" other button   ");
   }
   }
   irrecv.resume();

   return val;
}

void translateIR() // takes action based on IR code received
{
  // Check if it is a repeat IR code 
  //if (irrecv.decodedIRData.flags)
  //{
  //  //set the current decodedRawData to the last decodedRawData 
  //  irrecv.decodedIRData.decodedRawData = last_decodedRawData;
  //  Serial.println("REPEAT!");
  //} else {
    last_decodedRawData = irrecv.decodedIRData.decodedRawData;

    //output the IR code on the serial monitor
    Serial.print("decodedRawData IR code:0x");
    Serial.println(last_decodedRawData, HEX);
    //Serial.print("previous_decodedRawData IR code:0x");
    //Serial.println(previous_decodedRawData, HEX);
  //}
  //if (acceptNew) {
  //  expectedResponse = last_decodedRawData;
  //  acceptNew = 0;
  //}

  //map the IR code to the remote key
  uint32_t val = 0;
  switch (last_decodedRawData)
  {
    case 0xBA45FF00: Serial.println("POWER"); resetInput(); break;
    case 0xB847FF00: Serial.println("FUNC/STOP"); break;
    case 0xB946FF00: Serial.println("VOL+"); break;
    case 0xBB44FF00: Serial.println("FAST BACK");    break;
    case 0xBF40FF00: Serial.println("PAUSE");    break;
    case 0xBC43FF00: Serial.println("FAST FORWARD");   break;
    case 0xF807FF00: Serial.println("DOWN");    break;
    case 0xEA15FF00: Serial.println("VOL-");    break;
    case 0xF609FF00: Serial.println("UP");    break;
    case 0xE619FF00: Serial.println("EQ");    break;
    case 0xF20DFF00: Serial.println("ST/REPT");    break;
    case 0xE916FF00: Serial.println("0");    val=0; break;
    case 0xF30CFF00: Serial.println("1");    val=1; break;
    case 0xE718FF00: Serial.println("2");    val=2; break;
    case 0xA15EFF00: Serial.println("3");    val=3; break;
    case 0xF708FF00: Serial.println("4");    val=4; break;
    case 0xE31CFF00: Serial.println("5");    val=5; break;
    case 0xA55AFF00: Serial.println("6");    val=6; break;
    case 0xBD42FF00: Serial.println("7");    val=7; break;
    case 0xAD52FF00: Serial.println("8");    val=8; break;
    case 0xB54AFF00: Serial.println("9");    val=9; break;
    default:
      Serial.println(" other button   ");
  }
  if (answerIdx >=5) {
    resetInput();
  }
  userInput5[answerIdx++]= val;  
  printInput();
  //delay needed?
  delay(250); // Do not get immediate repeat
}

int getMotorPos(int motorNum)
{
   Serial.println("get pos for motor:");
   Serial.println(motorNum);

   int motorPos = -1;
   int loopNum = 1;
   // TBD: add timeout or max loops for this loop so we don't loop forever
   // TBD: or...do this looping outside this function
   while (loopNum < 10) {
      // Send command to get motor position
      Serial1.println(motorNum);

      if (Serial1.available()) { // Check if data is available to read
         String receivedData = Serial1.readStringUntil('\n'); // Read until a newline character
         motorPos = strtol(receivedData.c_str(), NULL, 10);
         Serial.println("motor pos: ");
         Serial.println(motorPos);
         return motorPos;
      }
   }
}

void doRoom3()
{
   Serial.println("In Room 3");
   setLowAll(3);

   // Flash lights to indicate start of stair game
   for (int i=0;i<5; i++) {
      setHighAll(3);
      delay(100);
      setLowAll(3);
      delay(100);
   }

   int lastLedRoom3 = 3;
   int currentLedRoom3 = 3;
   int roomThreeLoopIdx = 1;
   Serial.println("Stepping to next stair...");
   int numCorrect=0;
   while (!roomCompleted[3]) {
      Serial.println("Room 3 LED:");
      Serial.println(currentLedRoom3);
      digitalWrite(roomThreeLeds[currentLedRoom3-1], HIGH);

      // Move to Room 2, current stair
      Serial.println(5);
      int motor1Pos = -1;
      // Loop until motor 1 at proper position or we time out (TBD)
      while (motor1Pos != currentLedRoom3) {
         motor1Pos = getMotorPos(1);
         delay(500);
      }

      Serial.println("GOT A CORRECT ANSWER IN ROOM 3------------------------------------------");
      if (++numCorrect >= numStairsToJump) {
         roomCompleted[3] = true;
      }
      digitalWrite(roomThreeLeds[currentLedRoom3-1], LOW);
      Serial.println(numCorrect);

      // Create next stair to go to
      while (currentLedRoom3 == lastLedRoom3) {
         currentLedRoom3 = random(1,6);
      }
      lastLedRoom3 = currentLedRoom3;
   }
   currentRoom=4;

   // Flash lights to indicate end of stair game
   for (int i=0;i<5; i++) {
      setHighAll(3);
      delay(100);
      setLowAll(3);
      delay(100);
   }
}

void doRoom4()
{
   Serial.println("In Room 4");
   setLowAll(4);
   // Flash lights to indicate start of rune game
   for (int i=0;i<5; i++) {
      setHighAll(4);
      delay(100);
      setLowAll(4);
      delay(100);
   }

   // For each LED, flash number to decode (Q: one at a time?)
   int lastLedCount = -1;
   for (int i=0;i<3; i++) {

      // Choose random number of times to flash
      int flashCount = random(2,8);
      while (flashCount == lastLedCount) {
         flashCount = random(2,8);
      }
      Serial.println("flashCount");
      Serial.println(flashCount);
      flashLed(roomFourLeds[i], 1000*runeFlashTimeSec, flashCount);

      int motor2Pos = -1;
      // Loop until motor 2 at proper position or we time out
      while (motor2Pos != flashCount) {
         motor2Pos = getMotorPos(2);
         if (motor2Pos > 1999) {
            motor2Pos = motor2Pos - 2000;
         }
         Serial.println("         in loop...motor2Pos=");
         Serial.println(motor2Pos);
         delay(1000);
      }
      lastLedCount = flashCount;
      Serial.println("GOT A CORRECT ANSWER IN ROOM 4------------------------------------------");
   }

   roomCompleted[4] = true;
   currentRoom=5;

   // Flash lights to indicate end of rune game
   for (int i=0;i<5; i++) {
      setHighAll(4);
      delay(100);
      setLowAll(4);
      delay(100);
   }
}
