
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
const unsigned short pinPORT_EXPANDER_IR_CS = 7;
const unsigned short pinPWM_RGB_CS = 8;
const unsigned short pinSCAN_TIMER0 = 2; 
const unsigned short pinSCAN_TIMER1 = 4;// for this test suite, this is being used as a GPIO for some general timing purposes
const unsigned short pinSCAN_TIMER2 = 5;
const unsigned short pinDAC_DIFF_AMP_BIAS = 6;
const unsigned short pinPWM_GREYSCALE_CLOCK = 3;
const unsigned short pinPWM_XLAT = 9;
const unsigned short pinPWM_BLANKING_PULSE = 10;
const unsigned short pinANALOG_SELECT0 = A0;
const unsigned short pinANALOG_SELECT1 = A1;
const unsigned short pinANALOG_SELECT2 = A2;
const unsigned short pinANALOG_SELECT3 = A3;
const unsigned short pinANALOG_MUX_READ0 = A4;
const unsigned short pinANALOG_MUX_READ1 = A5;
const unsigned short pinANALOG_MUX_READ2 = A6;

// PORT EXPANDERS
MCP IRLEDs  (0, pinPORT_EXPANDER_IR_CS);


void setup() {

  Serial.begin(115200);
  Serial.println("Setup Begin");

  // initializing hardware pins (and default states where necessary). Outputs should be low
  // by default, but I declare many of them anyways just to remind which state we want things
  // in at startup.
  pinMode(pinPORT_EXPANDER_IR_CS, OUTPUT);
  digitalWrite(pinPORT_EXPANDER_IR_CS, HIGH); // chip selects are traditionally active low
  pinMode(pinPWM_RGB_CS, OUTPUT);
  digitalWrite(pinPWM_RGB_CS, LOW); // chip selects are traditionally active low, but
  // this chip select is a bit strange (done using external gate logic), and is active high
  pinMode(pinSCAN_TIMER0, OUTPUT);
  pinMode(pinSCAN_TIMER1, OUTPUT);
  pinMode(pinSCAN_TIMER2, OUTPUT);
  pinMode(pinDAC_DIFF_AMP_BIAS, OUTPUT); // PWM output to set an analog value
  pinMode(pinPWM_GREYSCALE_CLOCK, OUTPUT); // these will probably be managed in the LED driver library
  pinMode(pinPWM_XLAT, OUTPUT); // these will probably be managed in the LED driver library
  pinMode(pinPWM_BLANKING_PULSE, OUTPUT); // these will probably be managed in the LED driver library
  pinMode(pinANALOG_SELECT0, OUTPUT);
  pinMode(pinANALOG_SELECT1, OUTPUT);
  pinMode(pinANALOG_SELECT2, OUTPUT);
  pinMode(pinANALOG_SELECT3, OUTPUT);
  pinMode(pinANALOG_MUX_READ0, INPUT); // these are the analog inputs
  pinMode(pinANALOG_MUX_READ1, INPUT);
  pinMode(pinANALOG_MUX_READ2, INPUT);

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
  IRLEDs.digitalWrite(0B0000100010111111); // just LED117 on for testing. a single '1' in the lower byte and a single
  // '0' in the higher byte will always turn on a single LED. while turning on multiple is possible, it is not always
  // straightforward, and the power draw can quickly become very large. One at a time is recommended.
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
    // network, so the first 8 need to be low, the second 8 need to be high to turn off all LEDs
    if (i < 9){IRLEDs.digitalWrite(i, LOW);}
    else {IRLEDs.digitalWrite(i, HIGH);}
  }
}


void setAnalogMux(int channel){
    // if we are worried about speed, these digital writes need to be replaced. (and from hardware, pin selects
  // need to end up on one port!)
  if (channel%2 == 1){digitalWrite(pinANALOG_SELECT0, HIGH);} else{digitalWrite(pinANALOG_SELECT0, LOW);}
  if ((channel/2)%2 == 1){digitalWrite(pinANALOG_SELECT1, HIGH);} else{digitalWrite(pinANALOG_SELECT1, LOW);}
  if ((channel/4)%2 == 1){digitalWrite(pinANALOG_SELECT2, HIGH);} else{digitalWrite(pinANALOG_SELECT2, LOW);}
  if ((channel/8)%2 == 1){digitalWrite(pinANALOG_SELECT3, HIGH);} else{digitalWrite(pinANALOG_SELECT3, LOW);}
}
