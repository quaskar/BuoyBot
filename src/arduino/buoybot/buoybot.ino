/*
  WiFi UDP Send and Receive String

  This sketch waits for a UDP packet on localPort using the WiFi module.
  When a packet is received an Acknowledge packet is sent to the client on port remotePort

  created 30 December 2012
  by dlf (Metodo2 srl)

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-udp-send-receive-string
 */

#include "serial.h"
#include "wifi.h"
#include "compass.h"
#include "esc.h"
#include <PID_v1.h>

#include "config.h"


/* **************************************************************** */
/* * Global Variables                                             * */
/* **************************************************************** */ 
char broadcastBuffer[256];

double Compass_Target = 0.0;
double Speed_Target = 0.0;

double Ctrl_Speed = 0.0;
double Ctrl_YawRate = 0.0;

// PID controller
PID PID_Rotation(&Compass_Value, &Ctrl_YawRate, &Compass_Target, PID_ROTATION_KP, PID_ROTATION_KI, PID_ROTATION_KD, DIRECT);
//PID PID_Speed(&Speed_Value, &Speed_Value, &Speed_Target,PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD, DIRECT);


/* **************************************************************** */
/* * Local Routines                                               * */
/* **************************************************************** */ 
void fillBroadcastBuffer()
{
  sprintf(broadcastBuffer, "c:%d;y:%f\ntc%d", (int)Compass_Value, Ctrl_YawRate, (int)Compass_Target);
}

/* **************************************************************** */ 
void parse_commandLine(String line) {

  // get operations
  if (line[0] == 'G') {
    if (line[1] == 'S' || line[1] == 'A') {        // get speed
      Serial.print("GS");
      Serial.println(Ctrl_Speed);
    }
    if (line[1] == 'C' || line[1] == 'A') {       // get compass
      Serial.print("GC");
      Serial.println(Compass_Value);
    }
  }

  // get target operations
  if (line[0] == 'T') {
    if (line[1] == 'S' || line[1] == 'A') {       // get target speed
      Serial.print("TS");
      Serial.println(Speed_Target);
    }
    if (line[1] == 'C' || line[1] == 'A') {       // set target compass
      Serial.print("TC");
      Serial.println(Compass_Target);
    }
  }
          
  // set target operations
  if (line[0] == 'S') {
    if (line[1] == 'S') {                         // set target speed
      Speed_Target = line.substring(2).toDouble();
      Serial.print("SS");
      Serial.println(Speed_Target);
    }
    else if (line[1] == 'C') {                    // set target compass
      Compass_Target = line.substring(2).toDouble();
      Serial.print("SC");
      Serial.println(Compass_Target);
    }
  }
}


/* **************************************************************** */
/* * Setup Routine                                                * */
/* **************************************************************** */ 
void setup() {

  Serial_setup(SERIAL_BAUDRATE);
  Wifi_setup(SECRET_SSID, SECRET_PASS);
  Compass_setup();
  Esc_setup();

  PID_Rotation.SetMode(AUTOMATIC);
  PID_Rotation.SetOutputLimits(-PID_ROTATION_LIMIT, PID_ROTATION_LIMIT);

  //PID_Speed.SetMode(AUTOMATIC);
  //PID_Speed.SetOutputLimits(-PID_SPEED_LIMIT, PID_SPEED_LIMIT);
}


/* **************************************************************** */
/* * Loop Routine                                                 * */
/* **************************************************************** */ 
void loop() {
  Compass_loop();
  PID_Rotation.Compute();
  //PID_Speed.Compute();
  Esc_loop(Speed_Target, Ctrl_YawRate);

  Wifi_loop();
  Serial_loop();

  fillBroadcastBuffer();
  //Wifi_sendLine(broadcastBuffer);
  //Serial_sendLine(broadcastBuffer);  

  if (Serial_lineReceived) {
    //Serial_sendLine((char*)Serial_lineData.c_str());
    parse_commandLine(Serial_lineData);
    Serial_releaseLine();
  }

  delay (LOOP_DELAY);
}
