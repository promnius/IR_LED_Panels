
// TODO: check that the port expanders 0/1 are not backwards
// figure out actual pin mapping
// figure out the magic number to turn on LED117

// IRAnalogCharacterizationTestSuite
// Kyle Mayer
// May 2019
// This firmware is designed to toggle a single IR LED on the test panel, with the intent of
// probing a bunch of analog nodes with an oscilliscope to determine how well (and how fast)
// the analog filter network is. It also includes some functionality for playing with the variable
// diff amps, and recording and printing the values available to the MCU, for comparison to the
// scope readings. It is a standalone piece of firmware.

// LIBRARY INCLUDES
#include <SPI.h> 
#include <MCP23S17.h>

// GLOBAL VARIABLES
// analog value of IR returning to LED47 on the board
int recieverLED47 = 0; // for the full code, this needs to become an array, 
// and numbered using an XY grid rather than using the schematic reference designator
int recieverGroup3 = 0; // for the other way of testing analog return, we have a group of 
// receivers all tied together in analog.


int i = 0; // thats right, I declared global counter variables, and have two different nameing schemes because sometimes I'm in a
int counter = 0; // different mood. Sue me.

// PIN DECLARATIONS
const unsigned short pinPORTEXPANDERCS = 6;
const unsigned short pinEXPANDERSRESET = A2;
const unsigned short pinANALOGSELECT0 = A1;
const unsigned short pinANALOGSELECT1 = A0;
const unsigned short pinANALOGSELECT2 = 4;
const unsigned short pinANALOGSELECT3 = 4;
const unsigned short pinANALOGMUXREAD0 = A6;
const unsigned short pinANALOGMUXREAD1 = A6;
const unsigned short pinANALOGMUXREAD2 = A6;

// PORT EXPANDERS
MCP IRLEDs  (0, pinPORTEXPANDERCS);


void setup() {

  Serial.begin(115200);
  Serial.println("Setup Begin");

  // initializing hardware pins (and default states where necessary).
  pinMode(pinEXPANDERSRESET, OUTPUT);
  pinMode(pinPORTEXPANDERCS, OUTPUT);
  digitalWrite(pinEXPANDERSRESET, HIGH);
  pinMode(pinANALOGSELECT0, OUTPUT);
  pinMode(pinANALOGSELECT1, OUTPUT);
  pinMode(pinANALOGSELECT2, OUTPUT);
  pinMode(pinANALOGSELECT3, OUTPUT);
  pinMode(pinANALOGMUXREAD0, INPUT);
  pinMode(pinANALOGMUXREAD1, INPUT);
  pinMode(pinANALOGMUXREAD2, INPUT);

  Serial.println("Initializing MCP port expanders");
  delay(10);
  initializePortExpanders();
  delay(10); 
  Serial.println("Port Expanders Ready");
  Serial.println("Setup Done.");
  Serial.println("");
  delay(500);
}

void loop() {
  // turn on the IR LED
  IRLEDs.digitalWrite(0B0000000011111111); // just LED117 on for testing
  delayMicroseconds(100);
  IRLEDs.digitalWrite(0B0000000011111111); // all off
  
  delay(100); // so we don't overheat. this is overkill, but for testing doesn't matter.
}


// set up all the port expanders for beginning their journey
void initializePortExpanders(){  
  IRLEDs.begin();

  for (i = 1; i <= 16; i++) {    // Since we are only workign with one bit at a time, use a loop to take action each pin (0-15)
    // THERE IS A MUCH FASTER WAY TO DO THIS USING WORDS, BUT WE DON'T CARE ABOUT SPEED
    IRLEDs.pinMode(i, OUTPUT);      // set all LEDs as outputs, driven LEDs all off (port expander drives a mutliplexed P/N Fet
    // network, so some need to be high, some low
    if (i < 9){IRLEDs.digitalWrite(i, LOW);}
    else {IRLEDs.digitalWrite(i, HIGH);}
  }
}


void setAnalogMux(int channel){
    // if we are worried about speed, these digital writes need to be replaced. (and from hardware, pin selects
  // need to end up on one port!)
  if (channel%2 == 1){digitalWrite(pinANALOGSELECT0, HIGH);} else{digitalWrite(pinANALOGSELECT0, LOW);}
  if ((channel/2)%2 == 1){digitalWrite(pinANALOGSELECT1, HIGH);} else{digitalWrite(pinANALOGSELECT1, LOW);}
  if ((channel/4)%2 == 1){digitalWrite(pinANALOGSELECT2, HIGH);} else{digitalWrite(pinANALOGSELECT2, LOW);}
  if ((channel/8)%2 == 1){digitalWrite(pinANALOGSELECT3, HIGH);} else{digitalWrite(pinANALOGSELECT3, LOW);}
}
