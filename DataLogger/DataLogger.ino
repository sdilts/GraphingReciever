/*
 * This program reads sensor inputs and sends them over serial when requested.
 * 
 * @author Stuart Dilts
 * @version 10.10.16
 */

//control bytes sent by the client program
#define RESPOND 17
#define RESET_COUNTER 18

int inByte = 0;         // incoming serial byte

//anything sent in the other direction is unsigned:
unsigned long secsFromReset = 0;
unsigned int test = 0;

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
      sendInt(test);
      sendInt(test +2);
      test = test + 1;
    } else if(inByte == RESET_COUNTER) {
        test = 0;
        secsFromReset = millis();
    }
  }
}
/**
 * Sends a two bit number over serial
 */
void sendInt(unsigned int data) {
  byte b0 = (byte)data;
  byte b1 = (byte)(data>>8);
  Serial.write(b0);
  Serial.write(b1);
}
/**
 * Sends a four bit number over serial
 */
void sendLong(unsigned long data) {
   byte b0 = (byte)data;
   byte b1 = (byte)(data>>8);
   byte b2 = (byte)(data>>16);
   byte b3 = (byte)(data>>24);
   Serial.write(b0);
   Serial.write(b1);
   Serial.write(b2);
   Serial.write(b3);
}

