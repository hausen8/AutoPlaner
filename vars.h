/*
 * Sketch name: AutoPlaner_1.3.ino
 * Based on: RemonteTable_Uno_1_1.ino
 * Author: Wyldix (contact via metabricoleur.com forum)
 * Date created: 2015-04-02
 * Date last modified: Tilman, 2020-03-25
 * Description: Program for digitally controlled height adjustmend of planer table
 */


long          hMin = MIN_HEIGHT * 10;                 // Minimum table height in 0.1 mm
long          hMax = MAX_HEIGHT * 10;                 // Maximum table height in 0.1 mm
long          hHome = HOME_HEIGHT * 10;               // Home table height in 0.1 mm
long          hSafe = SAFE_HEIGHT * 10;               // Safe table height in 0.1 mm
long          steps = (NUM_STP_MOTOR * MICRO_STP * SPINDLE_PULLEY_TEETH) / (10 * SPINDLE_PITCH * MOTOR_PULLEY_TEETH);      // Number of motor steps to lift the table by 0.1 mm
long          accel = ACCELERATION * steps * 10;      // Acceleration in mm/s^2 (200 = step/s^2; hence multiplication by STEPS)
long          emStop = 10 * accel;                    // Deceleration in case of emergency stop
long          maxSpeed = (MAX_SPEED/6) * steps;       // Maximum table lift speed in mm/min (Default: 500. This is the first number to be changed)


long          hTarget = 0;                            // Height entered by user
long          hCurrent = hHome;                       // Variable permanently containing the current table height
static byte   kpadState;                              // Key status (PRESSED, HOLD, RELEASED (default hold time >2s)
char          key;                                    // Key pressed
long          precedentMillis = 0;                    // Save the last time the screen was refreshed
bool          motorRun = false;                       // Flag indicating running motor
bool          motorStop = false;                      // Flag indicating emergency stop
int           dispBright;                             // Display brightness
static char   lastKey = NO_KEY;                       // Stores last key press
bool          lockMode = false;                       // Flag indicating whether control panel is locked or unlocked




