
// Yoga Breathalyzer. Using Arduino, WS2812/Neopixel-style LED strip, and Modern Device wind sensor:http://moderndevice.com/product/wind-sensor/

#include "FastSPI_LED2.h" //library for driving LED strips is here: http://code.google.com/p/fastspi/
#include <LiquidCrystal.h> //library for driving LCD screen

//set up LED strip
#define NUM_LEDS 120
struct CRGB leds[NUM_LEDS]; //holds RGB values for the LEDs

// set up max and min values for wind and temp1 sensors (both built into the Modern Device sensor)
int windmin;
int windmax;
int tempmin;
int tempmax;

//for the LCD screen, initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int sensorPin = 5; // temp2 sensor pin

// SMOOTHING: Define the number of samples for smoothing the sensor signal for the temp2 sensor
int reading=0;
const int numReadings = 20;
int index = 0;                  // the index of the current reading
int readings[numReadings] = {0};  // array to hold the sensor values, for each sensor
int total = 0;                  // the running total for each sensor
int average=0;

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
  tempmax = analogRead(A2); // calibrate temp1 sensor
  tempmin = tempmax - 30;   //this is a reasonable range....
  
   // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
}


void loop() {
  //read wind/temp1 sensor and output results to LED strip
  int windSensor = analogRead(A0); // read wind sensor value
  int tempSensor = analogRead(A2); // read temp1 sensor value
  
  if(windSensor<windmin) windSensor=windmin; // compress ends of scale to avoid going out of range
  if(windSensor>windmax) windSensor=windmax;

  if(tempSensor<tempmin) tempSensor=tempmin;
  if(tempSensor>tempmax) tempSensor=tempmin;
  
  int dist = map(windSensor, windmin,windmax,0,119); //remap wind value to length of array
  int colorval = map(tempSensor, tempmin,tempmax,0,250); //remap temp1 to some reasonable color values

  colorFill(colorval,10,dist,leds);     //fill strip with color
  for (int i=0; i < NUM_LEDS; i++) { //clear array for the next loop
    memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
   }
  
   //Read temp2 sensor, convert to C and F, and display results on LCD screen
   reading = analogRead(sensorPin);  
    //time smoothing
    total= total - readings[index];         
    readings[index] = reading;   // put sensor reading into array:  
    total= total + readings[index];   // add the reading to the total:
    average = total / numReadings;   // calculate the average:  
    float voltage = average * 5;  //Convert to C and F 
    voltage /= 1024.0; 
    float temperatureC = (voltage - 0.5) * 100 ;  
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
   //print out temp2 reading on the LCD screen, but don't update every cycle to avoid screen flicker
  if(index==0)
   {
    lcd.setCursor(0, 0);
    lcd.print(temperatureC);lcd.print(" C    ");
    lcd.setCursor(0, 1);
    lcd.print(temperatureF);lcd.print(" F    ");
   }
   index++;
   if(index>=numReadings) index=0;

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
  return(CRGB(r, g, b));    //my led strip actually has the colors in g, r, b order; may be a nonstandard strip?
}

