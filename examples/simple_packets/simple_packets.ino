/*
*  Packetizer example
*/


#include "Packetizer.h"


// the packetizer
Packetizer slicer;

// bufferlength of packetizer
size_t bufferLength;

// result, use for error handling
uint8_t result;



//----------------------------------------
// setup
//----------------------------------------
void setup()
{
  Serial.begin(9600);

  //---------------------------------------- 
  // setup packetizer
  // alternatively to a constructor with argument, use init to resize internal buffer
  result = slicer.init(128);
  if (result != pz_noErr)
  {
    //error handling
    Serial.println("error init");
  }
  
  // to make sure, we know the correct bufferlength
  bufferLength = slicer.getBufferSize();
  //----------------------------------------
  // set start condition with string
  result = slicer.setStartCondition("start");
  if (result != pz_noErr)
  {
    //error handling
    Serial.println("error setting start");
  }

  //----------------------------------------
  // set end condition with byte-array
  uint8_t b[] = {13, 10};
  result = slicer.setEndCondition(b, sizeof(b));
//  result = slicer.setEndCondition("end");
  if (result != pz_noErr)
  {
    //error handling
    Serial.println("error setting end");
  }
  
  
  //----------------------------------------
  // set callbacks
  slicer.onPacketStart(myOnPacketStart);
  slicer.onPacket(myOnPacket);
  
  // optional: set a callback for buffer-overflow
  slicer.onOverflow(myOnOverflow);
  
  
  //----------------------------------------
  // initially fill packetizer with data...
  
}


//----------------------------------------
// loop
//----------------------------------------
void loop()
{
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
      // append data to slicer
      result = slicer.appendData((uint8_t)inVal);
      if (result != pz_noErr)
      {
        //error handling
        Serial.println("error adding data");
      } 
    }
    
    
    
    
  }
}



//----------------------------------------
// callback when start condition was found
//----------------------------------------
void myOnPacketStart()
{
  Serial.println("packet start");
}


//----------------------------------------
// callback for messages
//----------------------------------------
void myOnPacket(uint8_t* _buffer, size_t _bufferSize)
{
  // buffer is caller-owned
  // copy data if you need it later
  Serial.println("packet:");
  Serial.write(_buffer, _bufferSize);
  Serial.println();
}


//----------------------------------------
// callback for buffer-oberflow
//----------------------------------------
void myOnOverflow(uint8_t* _buffer, size_t _bufferSize)
{
  // buffer is caller-owned
  // copy data, if you need it later
  Serial.println("overflow:");
  Serial.write(_buffer, _bufferSize);
  Serial.println();  
}
