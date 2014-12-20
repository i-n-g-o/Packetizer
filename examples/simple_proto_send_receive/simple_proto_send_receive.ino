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



//----------------------------------------
//----------------------------------------
// processing sketch
//----------------------------------------
//----------------------------------------
///**
// * Simple Write. 
// * 
// * Check if the mouse is over a rectangle and writes the status to the serial port. 
// */
//
//
//import processing.serial.*;
//
//Serial myPort;  // Create object from Serial class
//int val;        // Data received from the serial port
//boolean overRect = false;
//
//void setup() 
//{
//  size(200, 200);
//  // I know that the first port in the serial list on my mac
//  // is always my  FTDI adaptor, so I open Serial.list()[0].
//  // On Windows machines, this generally opens COM1.
//  // Open whatever port is the one you're using.
// 
//  //print out serial devices 
//  int count = 0;
//  for (String s : Serial.list()) {
//    println(count + ": " + s);
//    count++;
//  }
//
//  String portName = Serial.list()[9];
//  myPort = new Serial(this, portName, 9600);
//}
//
//void draw() {
//  background(255);
//  if (mouseOverRect() == true) {  // If mouse is over square,
//    fill(204);                    // change color and
//    
//    if (!overRect) {
//      sendByteInPacket(100);
//      overRect = true;
//    }
//  } else {                        // If mouse is not over square,
//    fill(0);                      // change color and
//    
//    if (overRect) {
//      sendByteInPacket(101);
//      overRect = false;
//    }
//  }
//  
//  rect(50, 50, 100, 100);         // Draw a square
//}
//
//
//void sendByteInPacket(int b) {
//  myPort.write("start");
//  myPort.write(b);
//  
//  byte e[] = {13, 10};
//  myPort.write(e);
//}
//
//boolean mouseOverRect() { // Test if mouse is over square
//  return ((mouseX >= 50) && (mouseX <= 150) && (mouseY >= 50) && (mouseY <= 150));
//}
