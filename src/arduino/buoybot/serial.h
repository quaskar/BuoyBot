
/* **************************************************************** */
/* * Global Variables                                             * */
/* **************************************************************** */ 
bool Serial_lineReceived = false;
String Serial_lineData;


/* **************************************************************** */
/* * Local Routines                                               * */
/* **************************************************************** */ 
void Serial_sendLine(char msg[])
{
  Serial.println(msg);
}

/* **************************************************************** */ 
void Serial_releaseLine() {
  Serial_lineData = "";
  Serial_lineReceived = false;
}


/* **************************************************************** */
/* * Setup Routine                                                * */
/* **************************************************************** */ 
void Serial_setup(int Baudrate) {

  //Initialize serial and wait for port to open:
  Serial.begin(Baudrate);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}


/* **************************************************************** */
/* * Loop Routine                                                 * */
/* **************************************************************** */ 
void Serial_loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    Serial_lineData += inChar;
    
    if (inChar == '\n') {
      Serial_lineReceived = true;
    }
  }
}

/* **************************************************************** */
/* * Asyncron Events                                              * */
/* **************************************************************** */ 
