#include "VidorGraphics.h"
#include "Vidor_GFX.h"

Vidor_GFX  vdclk;
//drawClock();
int x = 0;
int y = 0;
int prev_sec;
void setup() {
  Serial.begin(9600);

  // wait for the serial monitor to open,
  // if you are powering the board from a USB charger remove the next line
  while (!Serial);

  // Initialize the FPGA
  if (!FPGA.begin()) {
    Serial.println("Initialization failed!");
    while (1) {}
  }
  delay(4000);
  
 drawClockFace();
  
}

void loop()
{
  //drawClockFace();
  for (int sec = 0; sec < 60; sec++){ 
  x = 320 + 130 * sin(prev_sec *0.1048); 
  y = 280 - 130 * cos(prev_sec * 0.1048); 
  vdclk.drawLine(320, 280, x, y, vdclk.White());
  
  x = 320 + 130 * sin(sec *0.1048); 
  y = 280 - 130 * cos(sec * 0.1048); 
  vdclk.drawLine(320, 280, x, y, vdclk.Red());
  prev_sec = sec;
  delay(20); 
 }

}

void drawClockFace(){
 // Fill the screen with a white background
 vdclk.fillRect(0,0,640,480,vdclk.White());
 //Draw the clock face
 vdclk.fillCircle(320,280,150 ,vdclk.lightBlue());
 vdclk.fillCircle(320,280,140 ,vdclk.White());
 vdclk.fillCircle(320,280,20 ,vdclk.lightBlue());

 for (int m = 0; m < 360; m = m + 30){
      int x = 0;
      int y = 0;
      x = 320 + 140 * sin(m *0.0175); 
      y = 280 - 140 * cos(m * 0.0175);  
      vdclk.fillRect(x,y,4,4,vdclk.Red());
 }
}
