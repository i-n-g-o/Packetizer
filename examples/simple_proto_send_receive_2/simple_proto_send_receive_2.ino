/*
*  Packetizer example
*
*  implementing a simple protocol  to set values of output pins
*  shows how to use packetizer for byte-pairs
*/

#include "Packetizer.h"


// the packetizer with internal buffer size
Packetizer slicerRecv;

// bufferlength of packetizer
unsigned int bufferLength = 128;


int ledPin1 = 9;
int ledPin2 = 10;
int ledPin3 = 11;

int counter = 0;
boolean sendOn = true;


//----------------------------------------
// setup
//----------------------------------------
void setup()
{
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  
  Serial.begin(9600);

  //---------------------------------------- 
  // setup packetizer
  slicerRecv.init(bufferLength);
  
  
  // to make sure, we know the correct bufferlength
  bufferLength = slicerRecv.getBufferSize();
  
  
  //----------------------------------------
  // set start condition with string
  slicerRecv.setStartCondition("start");  

  //----------------------------------------
  // set end condition with byte-array
  byte b[] = {13, 10};
  slicerRecv.setEndCondition(b, sizeof(b));
//  result = slicerRecv.setEndCondition("end");
  
  
  //----------------------------------------
  // set callbacks
  slicerRecv.onPacket(myOnPacket);  
}


//----------------------------------------
// loop
//----------------------------------------
void loop()
{
  counter++;
  
  if (counter > 100) {
    
    if (sendOn) {
      sendOn = false;
      
      // send packet for led 1
      slicerRecv.sendStartCondition(Serial);
      Serial.write(9);
      Serial.write(255);   
      slicerRecv.sendEndCondition(Serial);
      
      // send packet for led 2
      slicerRecv.sendStartCondition(Serial);
      Serial.write(10);
      Serial.write(125);   
      slicerRecv.sendEndCondition(Serial);
      
      // send packet for led 3
      slicerRecv.sendStartCondition(Serial);
      Serial.write(11);
      Serial.write(0);   
      slicerRecv.sendEndCondition(Serial);
            
    } else {
      sendOn = true;
      
      // send packet for led 1
      slicerRecv.sendStartCondition(Serial);
      Serial.write(9);
      Serial.write(25);   
      slicerRecv.sendEndCondition(Serial);
      
      // send packet for led 2
      slicerRecv.sendStartCondition(Serial);
      Serial.write(10);
      Serial.write(0);   
      slicerRecv.sendEndCondition(Serial);
      
      // send packet for led 3
      slicerRecv.sendStartCondition(Serial);
      Serial.write(11);
      Serial.write(55);   
      slicerRecv.sendEndCondition(Serial);
      
    }
    
    counter = 0;
  }
  
  delay(20);
}



//----------------------------------------
// catch serial events
//----------------------------------------
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:    
    int inVal = Serial.read();    
    if (inVal > -1)
    {      
      // append data to slicerRecv
      slicerRecv.appendData((byte)inVal);
    }    
  }
}


//----------------------------------------
// callback for messages
//----------------------------------------
void myOnPacket(byte* arr, unsigned int arrSize)
{
  // safety
  if (arrSize < 2) return;
  
  // get led id and value
  byte pin = arr[0];
  byte value = arr[1];
 
  // write led
  analogWrite(pin, value);
}
