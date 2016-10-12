/*
 * This program sends test data over serial when requested.
 * Used for debugging communication issues and testing new 
 * features of the main arduino program, DataLogger.
 * 
 * @author Stuart Dilts
 * @version 10.10.16
 */

//control bytes sent by the client program
#define RESPOND 17
#define RESET_COUNTER 18
#define TERMINATE 19

int inByte = 0;         // incoming serial byte

//anything sent in the other direction is unsigned:
unsigned long secsFromReset = 0;
int test = 120;

void setup() {
  // start serial port at 9600 bps:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    if(inByte == RESPOND) {
      sendLong(millis() - secsFromReset);
      //sendLong(longTest);
      sendInt(test+2);
      sendInt(test +4);
      
      test = test + 1;
    } else if(inByte == RESET_COUNTER) {
        test = 120;;
        secsFromReset = millis();
    } else if(inByte == TERMINATE) {
      //explode
    }
  }
}
/**
 * Sends a two bit number over serial
 */
void sendInt(int data) {
  byte buf[2];
  buf[0] = (byte)data;
  buf[1] = (byte)(data>>8);
  Serial.write(buf, sizeof(buf)); 
}
/**
 * Sends a four bit number over serial
 */
void sendLong(unsigned long data) {
  byte buf[4];
  buf[0] = data & 255;
  buf[1] = (data >> 8)  & 255;
  buf[2] = (data >> 16) & 255;
  buf[3] = (data >> 24) & 255;
  Serial.write(buf, 4);
}

