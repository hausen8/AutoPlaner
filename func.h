/*
 * Sketch name: AutoPlaner_1.3.ino
 * Based on: RemonteTable_Uno_1_1.ino
 * Author: Wyldix (contact via metabricoleur.com forum)
 * Date created: 2015-04-02
 * Date last modified: Tilman, 2020-03-25
 * Description: Program for digitally controlled height adjustmend of planer table
 */


// Enter display brightness mode (old value is blinking, waiting for new value to be entered)
void prepDisp() {
  tDisplay.clear();
  tDisplay.blinkRate(2);                        // Set blink rate of target display
  tDisplay.writeDigitRaw(1, 0b01011110);        // "d"
  tDisplay.writeDigitRaw(2, 0b11);              // ":"
  tDisplay.writeDigitNum(3, (dispBright - 4));  // Old value
  tDisplay.writeDisplay();
}


// Changing display brightness (Values from 0-9)
void setDisp(int key) {
  tDisplay.blinkRate(0);                        // Stop blink rate
  tDisplay.print(key);                          // Display brightness value
  tDisplay.writeDisplay();
  
  dispBright = key + 4;                         // Get brightness value in the right range
  EEPROM.put(4, dispBright);                    // Save brightness to EEPROM
  tDisplay.setBrightness(dispBright);           // Set target display brightness
  cDisplay.setBrightness(dispBright);           // Set current display brightness
  delay(2000);                                  // Display brightness value for 2 more seconds, then return to operational mode
  lastKey = NO_KEY;                             // Delete flag
}


// Target Display when machine is locked
lockDisp() {
  tDisplay.writeDigitRaw(0, 0b01000000);        // "-"
  tDisplay.writeDigitRaw(1, 0b01000000);        // "-"
  tDisplay.writeDigitRaw(3, 0b01000000);        // "-"
  tDisplay.writeDigitRaw(4, 0b01000000);        // "-"
  tDisplay.writeDisplay();
}


// Prepare locking/unlocking (first "#", has to be confirmed by a second "#")
void prepLock() {
  lockDisp();                                   // Write "----"
  tDisplay.blinkRate(2);                        // Set blink rate of target display
}


// Execute locking/unlocking either after confirmation or automatically by suction hood switch
void execLock() {
  if (lockMode == true) {
    lockMode = false;                           // Unset lock flag
  }
  else {
    lockDisp();                                 // Write "----"
    lockMode = true;                            // Set lock flag
  }
  tDisplay.blinkRate(0);                        // Remove blink rate
  lastKey = NO_KEY;                             // Delete flag
}


// Escape function (display brightness or lock mode) without changing value
void escFunc() {
  tDisplay.blinkRate(0);
  lastKey = NO_KEY;
}


// Homing the table
void setHome() {
  motor.setAcceleration(emStop);                // Increased acceleration for fast braking
  motor.stop();                                 // Stop motor
  
  motor.setMaxSpeed(MAX_SPEED / 10);            // Lower speed for better accuracy
  motor.move(-steps);                           // Move motor backwards
  motor.run();
  
  if(digitalRead(HomePin) == LOW) {
    motor.stop();                               // Stop motor when home switch releases
    motor.setCurrentPosition(hHome * steps);    // Set home position as current position
    motor.setMaxSpeed(maxSpeed);                // Set speed and acceleration back to normal values
    motor.setAcceleration(accel);               // Due to some reason hHome differs by 0.1mm when acceleration is not set
    EEPROM.put(0, hHome);                       // Store current height in EEPROM
    displayDec(hHome, DISP_CURR);               // Update current display value
    stopMotor();                                // Call function to get back to operational mode
  }
}


// Run table to safe height when suction hood gets flipped
void safeHeight() {
  if (motor.currentPosition() < (hSafe * steps)) {
    displayDec(hSafe, DISP_TARG);
    motor.moveTo(hSafe * steps);
    motorRun = true;
  }
}





