/*
 * Sketch name: AutoPlaner_1.3.ino
 * Based on: RemonteTable_Uno_1_1.ino
 * Author: Wyldix (contact via metabricoleur.com forum)
 * Date created: 2015-04-02
 * Date last modified: Tilman, 2020-03-25
 * Description: Program for digitally controlled height adjustmend of planer table
 */


// Included files
#include <Keypad.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <EEPROM.h>
// #include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <math.h>
#include "config.h"
#include "vars.h"


// Display instances
Adafruit_7segment tDisplay = Adafruit_7segment();
Adafruit_7segment cDisplay = Adafruit_7segment();


// Keypad
const byte ROW = 4;                     // rows
const byte COL = 4;                     // colums

#ifdef SCHURTER
  char numTouches[ROW][COL] = {         // B = Brightness
    { 1 ,  2 ,  3 ,  4 },               // L = Lock
    { 5 ,  6 ,  7 ,  8 },               // S = Stop
    { 9 ,  0 , 'B', 'L'},               // C = Clear
    {'S', 'C', 'H', 'E'}                // H = Home
  };                                    // E = Enter
#else
  char numTouches[ROW][COL] = {
    { 1 ,  2 ,  3 , 'S'},
    { 4 ,  5 ,  6 , 'C'},
    { 7 ,  8 ,  9 , 'H'},
    {'B',  0 , 'L', 'E'}
  };
#endif

byte rowPins[ROW] = {Row1Pin, Row2Pin, Row3Pin, Row4Pin};   
byte colPins[COL] = {Col1Pin, Col2Pin, Col3Pin, Col4Pin};

Keypad numPad = Keypad(makeKeymap(numTouches), rowPins, colPins, ROW, COL);


// Definition of stepper motor (controlled by driver with step/dir), additional enable command: setEnablePin()
// Step: One transition from low to high = one step (can be changed by setPinsIverted())
// Dir: High = clockwise rotation
AccelStepper motor(1, StepPin, DirPin);


void setup() {
  Serial.begin(9600);
  
  // Initialization of displays and erasing their contents
  if (BRIGHTNESS) {
    EEPROM.get(4, dispBright);                      // Read stored brightness from EEPROM
  }
  else {
    dispBright = DISP_BRGT;                         // If BRIGHTNESS disabled, use value from config.h
  }
  
  tDisplay.begin(DISP_TARG_ADD);
  cDisplay.begin(DISP_CURR_ADD);
  
  tDisplay.setBrightness(dispBright);
  tDisplay.clear();
  tDisplay.writeDisplay();

  cDisplay.setBrightness(dispBright);
  cDisplay.clear();
  cDisplay.writeDisplay();
  
  // Motor initialization
  EEPROM.get(0, hCurrent);                          // Read stored height from EEPROM
  motor.setCurrentPosition(hCurrent * steps);       // Current start height (home)
  motor.setMaxSpeed(maxSpeed);                      // Initialization of max speed and acceleration of stepper motor
  motor.setAcceleration(accel);
  motor.setMinPulseWidth(100);                      // Set minimum pulse width
  motor.setPinsInverted(INVDIR, false, false);      // Possible Invertion of direction (step and enable remain at default values)
    
  // Initialization of keyboard events
  numPad.addEventListener(keypadEvent_num);         // Adding an event for numeric keypad
  
  // Display of values on startup
  displayDec(motor.currentPosition() / steps, DISP_CURR);
  displayDec(hTarget, DISP_TARG); 
}


void loop() {
  runMotor();
}


// Include additional functions that are not crucial
#include "func.h"


// Keyboard event
void keypadEvent_num(KeypadEvent key){
    kpadState = numPad.getState();
    delay(50);
    swOnState(key);
}


// Key functions
void swOnState(char key) {
  switch(kpadState) {
    case PRESSED: {
      if(key == 'B' && BRIGHTNESS) {                      // Brightness
        prepDisp();
        lastKey = key;
      }
      if(key == 'L' && LOCK) {                            // Lock/Unlock
        if (lastKey == key) {
          execLock();
        }
        else {
          prepLock();
          lastKey = key;
        }
      }
      else if(key == 'C') {                               // Clear
        hTarget = 0;
      }
      else if((key >= 0) && (key <= 9)) {                 // Digit
        if (lastKey == 'B') {
          setDisp(key);
        }
        else if (!lockMode) {
          hTarget = (hTarget * 10) + key;
        }
      }
      else if(key == 'E' && hTarget > 0) {                // Start/Enter
        if(hTarget > hMax) {                              // Correcting height to maximum value when exceeding
          hTarget = hMax; 
        } 
        else if(hTarget < hMin) {                         // Correcting height to minimum value when deceeding
          hTarget = hMin;
        }
        else {                                            // Run stepper motor
          motor.moveTo(hTarget * steps);
          motorRun = true;
        }
      }
      else if(key == 'S') {                               // Stop stepper motor or escape function
        if (lastKey == 'B' || lastKey == 'L') {
          escFunc();
        }
        else {
          stopMotor();
        }
      }
      else if (key == 'H' && HOMING) {                    // Run motor in positive direction until it hits the home switch
        motor.move((hHome + hMin) * steps);               // Maximum distance to run: From 0 to home
        motorRun = true;
      }
      break;
    } 
    default: break;
  }
  if (lastKey != 'B' && lastKey != 'L' && !lockMode) {
    displayDec(hTarget, DISP_TARG);
  }
}


// Decimal output to appropriate display
void displayDec(float height, byte display) {
  if(display == DISP_TARG) {
    tDisplay.print(height/10);
    tDisplay.writeDisplay();
  }
  else {
    cDisplay.print(height/10);
    cDisplay.writeDisplay();
  }
}


// Emergency stop triggered by pressing the "STOP" button
void stopMotor() {
  if(motorRun) {
    motor.setAcceleration(emStop);              // Increased acceleration for fast braking
    motor.stop();                               // Send stop command
    motorStop = true;                           // Report emergency stop to main loop
  } 
}


void runMotor() {
  unsigned long currentMillis = millis();                   // Time since the Arduino board was turned on
             
  key = numPad.getKey();                                    // Read keypad

  if (!lockMode) {
    if(motorRun) {
      if(motorStop) {
        motor.runToPosition();
        motorStop = false;
        motor.setAcceleration(accel);
      }
      
      hCurrent = round(motor.currentPosition() / steps);      // Current position of the motor (therefore of the table)
      
      if(HOMING && digitalRead(HomePin) == HIGH) {
        setHome();
      }
      else if(motor.distanceToGo() != 0) {
        motor.run();
      }
      else {
        motorRun = false;
        hTarget = 0;
        displayDec(hTarget, DISP_TARG);
        displayDec(hCurrent, DISP_CURR);
        EEPROM.put(0, hCurrent);                              // Store current height in EEPROM
      }
      
      if(currentMillis - precedentMillis > INTERVAL) {        // Screen refresh only if the engine is running
        precedentMillis = currentMillis;
        displayDec(hCurrent, DISP_CURR);
      }
    }
    else if(HOOD && digitalRead(HoodPin) == HIGH) {
      safeHeight();
      if (motor.distanceToGo() == 0) {
        execLock();
      }
    }
  }
}





