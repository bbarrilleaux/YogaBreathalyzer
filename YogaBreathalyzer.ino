
// Yoga Breathalyzer. Using Arduino, WS2812/Neopixel-style LED strip, and Modern Device wind sensor:http://moderndevice.com/product/wind-sensor/

#include "FastSPI_LED2.h" //library for driving LED strips is here: http://code.google.com/p/fastspi/
#include <LiquidCrystal.h>

#define NUM_LEDS 120
struct CRGB leds[NUM_LEDS]; //holds RGB values for the LEDs

// set up max and min values for wind and temp sensors (both built into the Modern Device sensor)
int windmin;
int windmax;
int tempmin;
int tempmax;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int sensorPin = 5; // palm temp sensor pin



void setup() {
  //Set up LED strip:
  LEDS.setBrightness(250); // reduce total brightness of LED strip to reduce power consumption
  LEDS.addLeds<WS2811, 6>(leds, NUM_LEDS); // set up LED strip on pin 5
  
  //Calibrate wind sensor:
  colorFill(10,10,NUM_LEDS,leds);     //fill strip with color while waiting to calibrate
  delay(10000);                       //wait 10 seconds for temp sensor to stabilize
  Serial.begin(9600);
  windmin = analogRead(A0); // calibrate wind sensor
  windmax = windmin+300;    //this is a reasonable range....
  tempmax = analogRead(A2); // calibrate temp sensor
  tempmin = tempmax - 30;   //this is a reasonable range....
  
   // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
 
}


void loop() {
  int windSensor = analogRead(A0); // read wind sensor value
  int tempSensor = analogRead(A2); // read temp sensor value
  
  if(windSensor<windmin) windSensor=windmin; // compress ends of scale to avoid going out of range
  if(windSensor>windmax) windSensor=windmax;

  if(tempSensor<tempmin) tempSensor=tempmin;
  if(tempSensor>tempmax) tempSensor=tempmin;
  
  int dist = map(windSensor, windmin,windmax,0,119); //remap wind value to length of array
  int colorval = map(tempSensor, tempmin,tempmax,0,250); //remap temp to some reasonable color values

  colorFill(colorval,10,dist,leds);     //fill strip with color
  for (int i=0; i < NUM_LEDS; i++) { //clear array 
    memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
   }
  
  //Palm temperature sensor:
 int reading = analogRead(sensorPin);  
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100) 
 // now convert to Fahrenheit
 float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
 //print out palm temperature on the LCD screen
  lcd.setCursor(0, 0);
  lcd.print(temperatureC);lcd.print(" C    ");
  lcd.setCursor(0, 1);
  lcd.print(temperatureF);lcd.print(" F    ");

}
 
 
 
 
// Fill the strip with color c, up to a point determined by the IR sensor value.
void colorFill(uint32_t c, uint8_t wait, uint8_t distance, struct CRGB *leds) {
  int i;
 
  for (i=0; i <= distance; i++) {
      leds[i] = Wheel(c); // set all active pixels on
  }
  LEDS.show();
  delay(wait);
 }

/* Color wheel helper function */
//Input a value 0 to 256 to get a color value. My LED strip has the colors in a different order, oddly.
CRGB Wheel(uint16_t WheelPos) {
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      b = 127 - WheelPos % 128; // blue down
      g = WheelPos % 128;       // green up
      r = 0;                    // red off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
   }
  return(CRGB(r, g, b));    
}

