// Fire2012WithPaletteCycle
// Version 0.12 Revision 3
 
// Portions of this code originated in the FastLED library, and was
// modified & amended by Charles Gantt on 7/1/2017. Visit Charles' website
// at http://www.themakersworkbench.com
 
// Origial Fire2012 Code by:
// Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
 
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function 
// with additional custom defined color palettes. 
// The different palettes can be cycled through by a push of a button
// connected to pin 2, and 5V.

#include <FastLED.h>

#define LED_PIN     6         // the pin which attaches to the neopixel data pin
#define COLOR_ORDER GRB      // sets the color order in which the LEDs are designed for
#define CHIPSET     WS2811  // the chipset that the neopixels use
#define NUM_LEDS    8      // how many leds are being adderessed

#define BRIGHTNESS  200         // sets the overall brightness of the leds
#define FRAMES_PER_SECOND 60    // how fast should the led animation render

int switchPin = 2;              // switch is connected to pin 2
int val;                        // variable for reading the pin status
int buttonState;                // variable to hold the button state
int buttonPresses = 0;          // how many times the button has been pressed

bool gReverseDirection = false;  // set this to true if you need to invert the animation direction

CRGB leds[NUM_LEDS];     // sets up an array that we can manipulate to set/clear led data.

CRGBPalette16 currentPalette; // sets a variable for CRGBPalette16 which allows us to change this value later

void setup() {
  delay(3000); // sets a delay to insure that everything has initalized before proceeding with the setup

  // Informs the libraty that a strand of NEOPIXEL's on pin 6 and those leds will use the led array "leds", and there are NUM_PIXELS (aka 8) of them.
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); 
   
  FastLED.setBrightness( BRIGHTNESS );   // sets the brightness to the predetermined levels
  pinMode(switchPin, INPUT);    // Set the switch pin as input
  Serial.begin(9600);           // Set up serial communication at 9600bps
  buttonState = digitalRead(switchPin);   // read the initial state of the button 
}

void loop()
{
  random16_add_entropy( random());  // Add entropy to random number generator; we use a lot of it.

   // The following if statements read the number of times the button has been pressed
   // and then sets the currentPalette variable to the defined CRGBPalette16 color palette. 
   // for example: if the button has been pressed 1 time, set the palette to HeatColors. If
   // the button has been pressed 4 times, set the palette to a custom defined palette found in
   // that statement. The else statement at the end helps us return to the top of the if statments. 
  
   if (buttonPresses == 0) {
    currentPalette = CRGBPalette16( CRGB::Black);
    }
  if (buttonPresses == 1) {
    currentPalette = HeatColors_p;
  }
  if (buttonPresses == 2) {
    currentPalette = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White);
  }
  if (buttonPresses == 3) {
    currentPalette = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::GreenYellow,  CRGB::Yellow);
  }
  if (buttonPresses == 4) {
    currentPalette = CRGBPalette16( CRGB::Black, CRGB::Green, CRGB::Purple,  CRGB::Orange);
  }
  if (buttonPresses == 5) {
    currentPalette = CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Purple,  CRGB::Red);
  } 
  if (buttonPresses == 6) {
    currentPalette = CRGBPalette16( CRGB::White);
  } 
  if (buttonPresses == 7) {
    currentPalette = CRGBPalette16( CRGB::Yellow);
  }
  if (buttonPresses == 8) {
    currentPalette = CRGBPalette16( CRGB::Maroon);
  }
  if (buttonPresses == 9) {
    currentPalette = CRGBPalette16( CRGB::DarkBlue);
  }
  if (buttonPresses == 10) {
    static uint8_t hue = 0;
    hue++;
    CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
    CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
    currentPalette = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);
  }
  else  {
  }
  

  Fire2012WithPalette(); // calls the function defined below this loop & runs simulation frames, using palette colors
  
  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND); // sets a delay using the number 1000 devided by the predetermined frames per second. 

  val = digitalRead(switchPin);      // read input value and store it in val
  if (val != buttonState) {          // the button state has changed!
    if (val == LOW) {                // check if the button is pressed
      buttonPresses++;               // increment the buttonPresses variable
      Serial.print("Button has been pressed ");  // prints the statement between "" in the serial terminal
      Serial.print(buttonPresses);              // reads the value stored in the buttonPresses variable.
      Serial.println(" times");                // prints the statement between "" in the serial terminal
    }
  }
  if (buttonPresses > 10){    // reads the number of button presses, and if that number is greater than 10 then
  buttonPresses = 0;         // reset the number of button presses to 0  
  }
  buttonState = val;                 // save the new state in our variable

}

#define COOLING  55      // defines the level at which the lighting effect fades before a new "flame" generates

#define SPARKING 120    // defines the rate of flicker which we will see from the flame animation


void Fire2012WithPalette()   // defines a new function 
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 248);
      leds[j] = ColorFromPalette( currentPalette, colorindex);
    }
}
