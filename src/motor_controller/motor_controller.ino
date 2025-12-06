#include <stdlib.h>
#include <Wire.h>
#include <Encoder.h>
#include <Adafruit_MotorShield.h>

bool DEBUG = false;

// JOYSTICK PARAMETERS
const int jHorzPin = A1;
const int jVertPin = A0;
int jHorzValue = 0;
int jVertValue = 0;
int jDeadzone = 3; //deg
float sf1 = 0.8; //fraction of full 12V for M1
float sf2 = 0.5; //fraction of full 12V for M2
int jVertDir = 1;
int jHorzDir = -1;

// MOTION PARAMETERS
float Kp = 6;
float Kd = 10;
float tstep = 0.010; //sec
float res1 = 4261.449; //ecnts/mm (measured)
float res2 = 1424; //ecnt/deg (measured)
float inrange1 = 1; //mm
float tsettle = 0.1; // sec

// float Ki = 0;
// float res1 = 1496; //ecnt/rev (11 ppr * 4 * 34/1 ratio)
// float res2 = 1496; 

// POSITIONS
float POS_ROOM_1    = 0; //mm
float POS_ROOM_2_1  = 119.1;
float POS_ROOM_2_2  = 154.5;
float POS_ROOM_2_3  = 186.1;
float POS_ROOM_2_4  = 219.5;
float POS_ROOM_2_5  = 251.8;
float POS_ROOM_3    = 405;

float stepWidth = 10;

// SPEEDS
float SPEED_TYP = 240; //mm/s
float ACCEL_TYP = 1500; //mm/s/s

// MOTOR AND ENCODER SETUP
Adafruit_MotorShield afms = Adafruit_MotorShield();
Encoder enc1(2, 4); 
Encoder enc2(3, 5); 
Adafruit_DCMotor *m1 = afms.getMotor(3);
Adafruit_DCMotor *m2 = afms.getMotor(4);

// STATE
int state = 0;
int oldstate = 0;
bool newstate = true;
float posCmd = 0;
float spdCmd = 0;

// // COMMUNICATION
// int cmd = -1;

void setup() {
  
  afms.begin(); // motor shield init
  setSpeed(0);
  
  // Wire.begin(9); // i2c init
  // Wire.onReceive(receiveEvent); // receive interrupt

  Serial.begin(9600);
  debugPrint("");
  debugPrint("");
  debugPrint("Initialized and watiing for commands");
  
}



///////////////////////////////////////////////////////////////////////////////
// STATE SELECTION


// misc variables declared here
float hx = 0; //setpoint (deg)
float ht1 = 0;
float ht0 = 0;
float hx1 = 0;
float hx0 = 0;
float he1 = 0;
float he0 = 0;

float jx = 0;

String incomingString;

void loop() {

  if (Serial.available() > 0) {
    incomingString = Serial.readStringUntil('\n');
    incomingString.trim();
    debugPrint("cmd: " + incomingString);
    state = incomingString.toInt();

    // handle position requests separately without setting new state
    if (state == 1) {
        Serial.println(String(getStairNumber()));
        Serial.flush();
        state = oldstate;

    } else if (state == 2) {
        Serial.println(String(getRuneNumber()));
        Serial.flush();
        state = oldstate;

    } else {
      newstate = true;
        m1->run(RELEASE);
        m2->run(RELEASE);
      
    }

    oldstate = state;

  }



  // if (Serial.available() > 0) {
  //   String teststr = Serial.readString();
  //   teststr.trim();
  //   Serial.println(teststr);
  // }

  // if (Serial.available()) {
  //   String receivedData = Serial.readStringUntil("\n");
  //   Serial.println(">> " + receivedData);
  //   cmd = receivedData.toInt();
  //   state = cmd;
  //   newstate = true;
  // }

  switch(state) {

    case 0:
    //////////////////////////////// -1: Idle
      newstate = false;
      delay(5);
      break;
    
    case 3:
    //////////////////////////////// 3: Set axis 1 to joystick control

      if (newstate) {
        Serial.println("0");
        newstate = false;
      }
      
      // jx = getPos1();
      jVertValue = (analogRead(jVertPin) - 512) / 2 * jVertDir;
      debugPrint(String(jVertValue));

      if (jVertValue > jDeadzone) {
        m1->setSpeed(abs(jVertValue * sf1));
        m1->run(FORWARD);

      } else if (jVertValue < -jDeadzone) {
        m1->setSpeed(abs(jVertValue * sf1));
        m1->run(BACKWARD);

      } else {
        m1->run(RELEASE);
        
      }

      delay(tstep * 1000);
      break;

    case 4:
    //////////////////////////////// 4: Set axis 2 to joystick control

      if (newstate) {
        Serial.println("0");
        newstate = false;
      }
      
      // jx = getPos();
      jHorzValue = (analogRead(jHorzPin) - 512) / 2 * jHorzDir;
      debugPrint(String(jHorzValue));

      if (jHorzValue > jDeadzone) {
        m2->setSpeed(abs(jHorzValue * sf2));
        m2->run(FORWARD);

      } else if (jHorzValue < -jDeadzone) {
        m2->setSpeed(abs(jHorzValue * sf2));
        m2->run(BACKWARD);

      } else {
        m2->run(RELEASE);
        
      }

      delay(tstep * 1000);
      break;


    case 10:
    //////////////////////////////// 10: Move axis 1 to room 1     
      if (newstate) {
        moveToPosition(POS_ROOM_1, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;
    
    case 11:
    //////////////////////////////// 11: Move axis 1 to room 2.1
      if (newstate) {
        moveToPosition(POS_ROOM_2_1, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;

    case 12:
    //////////////////////////////// 12: Move axis 1 to room 2.2
      if (newstate) {
        moveToPosition(POS_ROOM_2_2, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;

    case 13:
    //////////////////////////////// 13: Move axis 1 to room 2.3
      if (newstate) {
        moveToPosition(POS_ROOM_2_3, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;

    case 14:
    //////////////////////////////// 13: Move axis 1 to room 2.4
      if (newstate) {
        moveToPosition(POS_ROOM_2_4, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;

    case 15:
    //////////////////////////////// 13: Move axis 1 to room 2.5
      if (newstate) {
        moveToPosition(POS_ROOM_2_5, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;

    case 16:
    //////////////////////////////// 13: Move axis 1 to room 3
      if (newstate) {
        moveToPosition(POS_ROOM_3, SPEED_TYP, ACCEL_TYP);
        Serial.println("0");
        newstate = false;
      }
      break;









    default: 
      Serial.println("-1");
      state = 0;
      newstate = true;
      break;
  }

}






// I2C RECEIVE
// void receiveEvent(int bytes) {

//   String rcdCmd;
//   while (Wire.available()) // loop through all but the last
//   {
//     char c = Wire.read(); // receive byte as a character
//     rcdCmd += c;
//   }
//   Serial.print("Received ");
//   Serial.println(rcdCmd);

//   if (rcdCmd[0] == 3) {
//     state = 3;

//     int i1, i2;

//     i1 = rcdCmd.indexOf(" ") + 1;
//     i2 = rcdCmd.substring(i1+1,rcdCmd.length()).indexOf(" ") + 1;

//     posCmd = rcdCmd.substring(i1, i1+i2-1).toFloat();
//     spdCmd = rcdCmd.substring(i1+i2-1, rcdCmd.length()).toFloat();

//     Serial.println("Command: pos " + String(posCmd) + " spd " + String(spdCmd));

//   } else {
//     state = rcdCmd.toInt();
//   }

//   newstate = true;
// }




///////////////////////////////////////////////////////////////////////////////
// HIGH LEVEL MOTION COMMANDS

void holdPosition(int time) {

  // Holds motor 1 in position for a given time.

  int nsteps = time / tstep;

  float x = getPos1(); //setpoint (deg)

  float t1 = 0;
  float t0 = 0;
  float x1 = 0;
  float x0 = 0;
  float e1 = 0;
  float e0 = 0;

  int i;
  for (i = 0; i < nsteps; i++) {

    t1 = millis();
    x1 = getPos1();
    e1 = x1 - x;

    applyPID(x, t1, t0, x1, x0, e1, e0);

    while(millis() - t1 < tstep * 1000) {
      // wait
    }

    x0 = x1;
    t0 = t1;
    e0 = e1;

  }
  setSpeed(0);
}

void setPosition(float x) {

  // Attempts to move motor 1 to the given setpoint x instantly. Holds until 
  // settled.

  int scnt = 0; // settling counter
  int nscnt = int(tsettle / tstep); // settling count required

  float t1 = 0;
  float t0 = 0;
  float x1 = 0;
  float x0 = 0;
  float e1 = 0;
  float e0 = 0;

  int i;
  while (scnt < nscnt) {

    t1 = millis();
    x1 = getPos1();
    e1 = x1 - x;
    
    if (abs(x1 - x) < inrange1) {
      scnt++;
    }

    applyPID(x, t1, t0, x1, x0, e1, e0);

    while(millis() - t1 < tstep * 1000) {
      // wait
    }

    x0 = x1;
    t0 = t1;
    e0 = e1;

  }
  setSpeed(0);
}

void moveToPosition(float x_sp, float v_max, float a_max) {
  
  // Moves motor 1 from current position to x_sp using a trapezoidal profile.
  // Does not return until settled.

  int scnt = 0; // settling counter
  int nscnt = int(tsettle / tstep); // settling count required

  float t1 = 0;
  float t0 = 0;
  float x1 = 0;
  float x0 = 0;
  float e1 = 0;
  float e0 = 0;

  float tstart = float(millis());
  float xstart = getPos1();
  float t = 0;
  float x = xstart; // "x" will be the generated setpoint

  float m;
  if (x_sp >= xstart) {
    m = 1;
  } else {
    m = -1;
  }

  while (abs(x - x_sp) > inrange1) {

    t = (millis() - tstart) / 1000;
    x = xstart + m * getPosFromTrapProf(a_max, v_max, abs(x_sp - xstart), t);
    // setpoint from trap prof

    t1 = millis();
    x1 = getPos1();
    e1 = x1 - x;
    
    if (abs(x1 - x) < inrange1) {
      scnt++;
    }

    applyPID(x, t1, t0, x1, x0, e1, e0);

    while(millis() - t1 < tstep * 1000) {
      // wait
    }

    x0 = x1;
    t0 = t1;
    e0 = e1;
  }
  setPosition(x_sp);
}


///////////////////////////////////////////////////////////////////////////////
// PROFILE GENERATION

float getPosFromTrapProf(float a_max, float v_max, float x_max, float t) {

  // Extremely simple trapezoidal motion profile generation
  // Returns positive x value between 0 and x_max given t
  // Note: this does NOT do any verification whatsoever. Possible jump if 
  // requested profile isn't possible

  float x;
  float x_ramp;
  float t_ramp;
  float t_max;

  t_ramp = v_max / a_max;
  t_max = x_max / v_max + t_ramp;
  x_ramp = 0.5 * v_max * t_ramp;

  if (t < t_ramp) {
    x = 0.5 * a_max * t * t;

  } else if (t < t_max - t_ramp) {
    x = v_max * (t - t_ramp) + x_ramp;

  } else {
    x = x_max - 0.5 * a_max * (t - t_max) * (t - t_max);

  }
  return x;
}



///////////////////////////////////////////////////////////////////////////////
// MOTOR LEVEL FUNCTIONS

void applyPID(float x, float t1, float t0, float x1, float x0, float e1, float e0) {

  // Applies torque in response to position error

  float de1 = (e1 - e0) / (t1 - t0);

  float cmd_p = Kp * e1;
  float cmd_d = Kd * de1;
  float cmd = cmd_p + cmd_d;
  
  // String s = String(String(t1) + "\t" + String(x1) + "\t" + String(e1) + "\t" + String(cmd_p) + "\t" + String(cmd_d));    
  // Serial.println(s);
  
  setSpeed(cmd);
}

void setSpeed(float spd) {
  //-100% to 100%

  if (spd > 100) { spd = 100; }
  if (spd < -100) { spd = -100; }

  if (spd == 0) {
    m1->setSpeed(0);
    m1->run(RELEASE);  

  } else {
    if (spd > 0) {
      m1->run(BACKWARD);
    } else {
      m1->run(FORWARD);
    }
    m1->setSpeed(abs(spd)*2.55);
  }
}

float getPos1() {
  return float(enc1.read()) * 360 / res1;
}

int getRuneNumber() {
  float d = float(enc2.read()) * 360 / res2;
  debugPrint(String(d));
  int rune = (int(d) + 30) % 360;
  rune += rune < 0 ? 360 : 0;
  rune = rune / 60 + 1;
  debugPrint(String(rune));
  return rune;
}

int getStairNumber() {
  float p = getPos1();
  debugPrint(String(p));
  int step = -1;

  if (p > POS_ROOM_2_1 - stepWidth && p < POS_ROOM_2_1 + stepWidth) {
    step = 1;

  } else if (p > POS_ROOM_2_2 - stepWidth && p < POS_ROOM_2_2 + stepWidth) {
    step = 2;

  } else if (p > POS_ROOM_2_3 - stepWidth && p < POS_ROOM_2_3 + stepWidth) {
    step = 3;

  } else if (p > POS_ROOM_2_4 - stepWidth && p < POS_ROOM_2_4 + stepWidth) {
    step = 4;

  } else if (p > POS_ROOM_2_5 - stepWidth && p < POS_ROOM_2_5 + stepWidth) {
    step = 5;

  } else {
    step = -1;

  }

  return step;
}


void debugPrint(String s) {
  if (DEBUG) {
    Serial.println("> " + s);
  }
}

