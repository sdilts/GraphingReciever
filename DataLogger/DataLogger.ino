/*
 * This program reads sensor inputs and sends them over serial when requested.
 * 
 * @author Stuart Dilts
 * @version 10.10.16
 */

//control bytes sent by the client program
#define RESPOND 17
#define RESET_COUNTER 18
#define TERMINATE 19

//sensor ports:
#define SALT A0
#define PRESSURE A5

int inByte = 0;         // incoming serial byte

//anything sent in the other direction is unsigned:
unsigned long secsFromReset = 0;

void setup() {
  pinMode(SALT, INPUT);
  pinMode(PRESSURE, INPUT);
  
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
      sendInt(analogRead(SALT));
      sendInt(analogRead(PRESSURE));
      
    } else if(inByte == RESET_COUNTER) {
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

