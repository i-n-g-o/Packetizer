/*
*  Packetizer example
*
*  implementing a simple protocol to turn on and of led 13
*/

#include "Packetizer.h"


// the packetizer with internal buffer size
Packetizer slicerRecv;

// bufferlength of packetizer
unsigned int bufferLength = 128;


int ledPin = 13;

int counter = 0;
boolean sendOn = true;


//----------------------------------------
// setup
//----------------------------------------
void setup()
{
  pinMode(ledPin, OUTPUT);
  
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
      
      // send start condition
      slicerRecv.sendStartCondition(Serial);
      
      // send id to turn led on
      Serial.write(100);
      
      // send end condition
      slicerRecv.sendEndCondition(Serial);
      
      
    } else {
      sendOn = true;
      
      // send start condition
      slicerRecv.sendStartCondition(Serial);
      
      // send id to turn led off
      Serial.write(101);
      
      // send end condition
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
  // analyze buffer
  switch(arr[0])
  {
    case 100:
      // led on
      digitalWrite(ledPin, HIGH);
      break;
      
    case 101:
      // led off
      digitalWrite(ledPin, LOW);
      break;
      
    default:
      break;
  }
}
