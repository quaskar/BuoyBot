#include <Servo.h>
#include <PID_v1.h>
#include <QMC5883LCompass.h>

/* **************************************************************** */
/* * Configuration Parameters                                     * */
/* **************************************************************** */ 
#define SERIAL_BAUDRATE     9600

#define PID_ROTATION_KP     2
#define PID_ROTATION_KI     5
#define PID_ROTATION_KD     1
#define PID_ROTATION_LIMIT  5

#define PID_SPEED_KP        2
#define PID_SPEED_KI        5
#define PID_SPEED_KD        1
#define PID_SPEED_LIMIT     85


/* **************************************************************** */
/* * Global Variables                                             * */
/* **************************************************************** */ 
// Serial
String Serial_Buffer = "";

// Brushless motor controller
Servo ESC_Bb; 
Servo ESC_Stb;

// Compass
QMC5883LCompass Compass; 
double Compass_Target;
double Compass_Value = 0;

// Speed
double Speed_Target;
double Speed_Value = 0;

// Ctrl parametery
double Ctrl_YawRate = 0;

// PID controller
PID PID_Rotation(&Compass_Value, &Ctrl_YawRate, &Compass_Target,PID_ROTATION_KP, PID_ROTATION_KI, PID_ROTATION_KD, DIRECT);
PID PID_Speed(&Speed_Value, &Speed_Value, &Speed_Target,PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD, DIRECT);


/* **************************************************************** */
/* * Setup Routines                                               * */
/* **************************************************************** */ 
void setup_serial() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial_Buffer.reserve(20);

  // identify controller with version number
  Serial.print("\nBuoyBot Controller v?.?\n");
}

/* **************************************************************** */ 
void setup_compass() {
  Compass.init();
  Speed_Value = Compass.getAzimuth();
}

/* **************************************************************** */ 
void setup_esc() {
  ESC_Bb.attach  ( 9, 1000, 2000);
  ESC_Stb.attach (10, 1000, 2000);

  ESC_Bb.write  (0);
  ESC_Stb.write (0);

  delay(100);

  ESC_Bb.write  (90);
  ESC_Stb.write (90);  
}

/* **************************************************************** */ 
void setup_pid() {
  Compass_Target = Compass_Value;
  PID_Rotation.SetMode(AUTOMATIC);
  PID_Rotation.SetOutputLimits(-PID_ROTATION_LIMIT, PID_ROTATION_LIMIT);

  Speed_Target = Speed_Value;
  PID_Speed.SetMode(AUTOMATIC);
  PID_Speed.SetOutputLimits(-PID_SPEED_LIMIT, PID_SPEED_LIMIT);
}

/* **************************************************************** */ 
void setup() {
  setup_serial();
  setup_compass();
  setup_pid();
  setup_esc();
}


/* **************************************************************** */
/* * Loop Routines                                                * */
/* **************************************************************** */ 
void loop_compass() {
  Speed_Value = Compass.getAzimuth();
}

/* **************************************************************** */ 
void loop_pid() {
  PID_Rotation.Compute();
  PID_Speed.Compute();
}

/* **************************************************************** */ 
void loop_esc() {
  ESC_Bb.write (Speed_Value - Ctrl_YawRate);
  ESC_Stb.write(Speed_Value + Ctrl_YawRate);
}

/* **************************************************************** */ 
void loop() {
  loop_compass();
  loop_pid();
  loop_esc();
}


/* **************************************************************** */
/* * Asyncron Events                                              * */
/* **************************************************************** */ 
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    Serial_Buffer += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    
    // parse command if compete
    if (inChar == '\n') {

      // get operations
      if (Serial_Buffer[0] == 'g') {
        if (Serial_Buffer[1] == 's') {        // get speed
          Serial.print("GS");
          Serial.print(Speed_Value);
          Serial.print("\n");
        }
        else if (Serial_Buffer[1] == 'c') {   // get compass
          Serial.print("GC");
          Serial.print(Compass_Value);
          Serial.print("\n");
        }
      }

      // target operations
      if (Serial_Buffer[0] == 't') {
        if (Serial_Buffer[1] == 's') {        // set speed
          Serial.print("TS");
          Serial.print(Speed_Target);
          Serial.print("\n");
        }
        else if (Serial_Buffer[1] == 'c') {   // set compass
          Serial.print("TC");
          Serial.print(Compass_Target);
          Serial.print("\n");
        }
      }
      
      // set operations
      if (Serial_Buffer[0] == 's') {
        if (Serial_Buffer[1] == 's') {        // set speed
          Speed_Target = Serial_Buffer.substring(2).toInt();
        }
        else if (Serial_Buffer[1] == 'c') {   // set compass
          Compass_Target = Serial_Buffer.substring(2).toInt();
        }
      }
      Serial_Buffer = "";
    }
  }
}
