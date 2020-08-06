/*
 This example reads audio data from an Invensense's ICS43432 I2S microphone
 breakout board, and prints out the spectrum to the Serial console. The
 Serial Plotter built into the Arduino IDE can be used to plot the audio
 amplitude data (Tools -> Serial Plotter)

 Circuit:
 * Arduino/Genuino Zero, MKRZero or MKR1000 board
 * ICS43432:
   * GND connected GND
   * 3.3V connected 3.3V (Zero) or VCC (MKR1000, MKRZero)
   * WS connected to pin 0 (Zero) or pin 3 (MKR1000, MKRZero)
   * CLK connected to pin 1 (Zero) or pin 2 (MKR1000, MKRZero)
   * SD connected to pin 9 (Zero) or pin A6 (MKR1000, MKRZero)

 created 21 November 2016
 by Sandeep Mistry
 */
#include "VidorGraphics.h"
#include "Vidor_GFX.h"
#include <ArduinoSound.h>

Vidor_GFX  vdclk;

//Width and Height of Screen
const int width  = 640;
const int height = 480;

// sample rate for the input
const int sampleRate = 8000;

// size of the FFT to compute
const int fftSize = 64;

// size of the spectrum output, half of FFT size
const int spectrumSize = fftSize / 2;

// array to store spectrum output
int spectrum[spectrumSize];

// array to store previous output
static int prev_spectrum[spectrumSize];

// create an FFT analyzer to be used with the I2S input
FFTAnalyzer fftAnalyzer(fftSize);


//General Variables used in program
int counter = 0;

int maxVal = 350000;
int tog = 1;

char gX = 20, gY = 80;
int gWidth = 470, gHeight = 380;
bool clipping_flag = false;


void setup() {
  // Open serial communications and wait for port to open:
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // setup the I2S audio input for the sample rate with 32-bits per sample
  if (!AudioInI2S.begin(sampleRate, 32)) {
    Serial.println("Failed to initialize I2S input!");
    while (1); // do nothing
  }

  // configure the I2S input as the input for the FFT analyzer
  if (!fftAnalyzer.input(AudioInI2S)) {
    Serial.println("Failed to set FFT analyzer input!");
    while (1); // do nothing
  }

  // Initialize the FPGA
  if (!FPGA.begin()) {
    Serial.println("Initialization failed!");
    while (1) {}
  }
  //clear screen
  clearScreen();
  //delay for 1 second
  delay(1000);
  graphWindow(gX, gY, gWidth, gHeight);
}


void loop() {  
  static int yscale, xscale;
  static float mean_val;
  static float threshold;
  //take in sound and retrieve frequency bins
  // check if a new analysis is available
  //clearScreen();
  if (fftAnalyzer.available()) {
    // read the new spectrum
    fftAnalyzer.read(spectrum, spectrumSize);
    for(int i = 0; i < spectrumSize; i++) {
      mean_val +=spectrum[i];
    }
    mean_val /= spectrumSize;
    threshold = 1.1*mean_val;
    
    //print out the spectrum
    for (int i = 0; i < spectrumSize; i++) {
      //Scale to Screen Height
      //Scale spectrum bin to width
      //spectrum[i] = abs(spectrum[i]);
      
      yscale = map(spectrum[i], 1, maxVal, 1, gHeight);
      //yscale = abs(yscale);
      yscale = (prev_spectrum[i] + yscale) / 2;
      if(yscale > gHeight) {
        yscale = gHeight;
        maxVal +=25000;
        //Serial.println(maxVal);
        //clip max
        if(maxVal > 1000000) {
          maxVal = 1000000;
        }
      }
      xscale = map((i*sampleRate) / fftSize, 1, (spectrumSize*sampleRate) / fftSize, gX+1, gX+gWidth);
      //Serial.println(yscale);
      //Remove previous line
      vdclk.drawLine(xscale,  gY + gHeight - prev_spectrum[i], xscale, gY + gHeight, vdclk.White());
      //drawnew line;
      vdclk.drawLine(xscale, gY + gHeight - yscale, xscale, gY + gHeight, vdclk.Blue());
      prev_spectrum[i] = yscale;
    }
    clipping_flag = false;
    //tog = ~tog;
  }
  //wait
  //delay(1);
  //end loop
  //clearScreen();
}

void clearScreen() {
  // Fill the screen with a white background
  vdclk.fillRect(0,0,640,480,vdclk.White());
}

void graphWindow(char sX, char sY, int w, int h) {

  vdclk.drawRect(sX-2, sY-2, w+2, h+2, vdclk.Black());
 
  vdclk.text.setCursor(150,25);
  vdclk.text.setAlpha(255);
  vdclk.text.setSize(1);
  vdclk.text.setColor(vdclk.Black());
  vdclk.println("FFT Live Graph");
  vdclk.text.setCursor(gX, gY+gHeight+18);
  vdclk.println(0);
  vdclk.text.setCursor(gWidth - 15, gY+gHeight+18);
  vdclk.println((spectrumSize*sampleRate) / fftSize);
}
