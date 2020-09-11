/*
 * Sketch name: AutoPlaner_1.3.ino
 * Based on: RemonteTable_Uno_1_1.ino
 * Author: Wyldix (contact via metabricoleur.com forum)
 * Date created: 2015-04-02
 * Date last modified: Tilman, 2020-03-25
 * Description: Program for digitally controlled height adjustmend of planer table
 */


// User defined constants, depending on the planer
#define MIN_HEIGHT 6              // Minimum table height in millimeters
#define MAX_HEIGHT 230            // Maximum table height in millimeters
#define HOME_HEIGHT 230           // Home table height in millimeters (must be >= MAX_HEIGHT!)
#define SAFE_HEIGHT 170           // Save height for flipping the suction (if activated)
#define NUM_STP_MOTOR 200         // Number of steps per revolution of stepper motor
#define MICRO_STP 4               // Number of micro steps used by the motor driver 
#define SPINDLE_PITCH 2           // Pitch of the planer table spindle in millimeters
#define SPINDLE_PULLEY_TEETH 20   // Number of pulley teeth on the table spindle
#define MOTOR_PULLEY_TEETH 20     // Number of pulley teeth on the motor shaft
#define INVDIR false              // Direction of rotation of the stepper motor ("true" or "false")


// Speed profile (to be modified later, depending on desired speed)
#define ACCELERATION 2            // Acceleration in mm/s^2
#define MAX_SPEED 500             // Maximum table lift speed in mm/min (Default: 500. This is the first number to be changed)
#define INTERVAL 100              // Period between each screen refresh in ms


// Additional functions in func.h
#define HOMING true               // Homing function ("true" or "false")
#define HOOD true                 // Suction hood control ("true" or "false")
#define BRIGHTNESS true           // Set display brightness and save to EEPROM ("true" or "false")
#define LOCK true                 // Lock stepper motor for safety


// Display
#define DISP_TARG_ADD 0x70        // I2C address of display 1
#define DISP_CURR_ADD 0x74        // I2C address of display 2
#define DISP_TARG 1
#define DISP_CURR 2
#define DISP_BRGT 6              // Brightness value of displays from 0 to 15 (brightest)


// Keypad
#define SCHURTER                  // Uncomment for Schurter Keypads


// Microcontroller
#define MEGA                      // Uncomment for Arduino MEGA 2560, comment for Arduino UNO or LEONARDO

#ifdef MEGA
  #define Row1Pin 39              // Pins for keypad rows
  #define Row2Pin 41
  #define Row3Pin 43
  #define Row4Pin 45
  
  #define Col1Pin 47              // Pins for keypad colums
  #define Col2Pin 49
  #define Col3Pin 51
  #define Col4Pin 53
  
  #define StepPin 2               // Stepper motor pins 
  #define DirPin  38

  #define HoodPin 19              // Suction hood switch pin
  #define HomePin 50              // Home switch pin
#else
  #define Row1Pin 13              // Pins for keypad rows
  #define Row2Pin 12
  #define Row3Pin 11
  #define Row4Pin 10
  
  #define Col1Pin 9               // Pins for keypad colums
  #define Col2Pin 8
  #define Col3Pin 7
  #define Col4Pin 6
  
  #define StepPin 5               // Stepper motor pins 
  #define DirPin  4

  #define HoodPin 3               // Suction hood switch pin
  #define HomePin 2               // Home switch pin
#endif





