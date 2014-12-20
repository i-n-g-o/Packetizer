/**
 * Simple Write. 
 * 
 * Check if the mouse is over a rectangle and writes the status to the serial port. 
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
boolean overRect = false;

void setup() 
{
  size(200, 200);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
 
  //print out serial devices 
  int count = 0;
  for (String s : Serial.list()) {
    println(count + ": " + s);
    count++;
  }

  String portName = Serial.list()[9];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  background(255);
  if (mouseOverRect() == true) {  // If mouse is over square,
    fill(204);                    // change color and
    
    if (!overRect) {
      sendByteInPacket(100);
    }
   
    overRect = true;
    
  } 
  else {                        // If mouse is not over square,
    fill(0);                      // change color and
    
    if (overRect) {
      sendByteInPacket(101);
    }
    
    overRect = false;
    
  }
  rect(50, 50, 100, 100);         // Draw a square
}


void sendByteInPacket(int b) {
  myPort.write("start");
  myPort.write(b);
  
  byte e[] = {13, 10};
  myPort.write(e);
}

boolean mouseOverRect() { // Test if mouse is over square
  return ((mouseX >= 50) && (mouseX <= 150) && (mouseY >= 50) && (mouseY <= 150));
}
