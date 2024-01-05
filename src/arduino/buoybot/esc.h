#include <Servo.h>

/* **************************************************************** */
/* * Global Variables                                             * */
/* **************************************************************** */ 
Servo Esc_Bb; 
Servo Esc_Stb;


/* **************************************************************** */
/* * Setup Routine                                                * */
/* **************************************************************** */ 
void Esc_setup() {
  Esc_Bb.attach  ( 9, 1000, 2000);
  Esc_Stb.attach (10, 1000, 2000);

  Esc_Bb.write  (0);
  Esc_Stb.write (0);

  delay(100);

  Esc_Bb.write  (90);
  Esc_Stb.write (90);  
}


/* **************************************************************** */
/* * Loop Routine                                                 * */
/* **************************************************************** */ 
void Esc_loop(double speed, double yawrate) {
  Esc_Bb.write (speed - yawrate);
  Esc_Stb.write(speed + yawrate);
}
