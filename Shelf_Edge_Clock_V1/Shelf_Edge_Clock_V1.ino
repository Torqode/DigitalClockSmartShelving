/*
 * 3D printed smart shelving with a giant hidden digital clock in the front edges of the shelves - DIY Machines

==========

More info and build instructions: https://www.youtube.com/watch?v=8E0SeycTzHw

3D printed parts can be downloaded from here: https://www.thingiverse.com/thing:4207524

You will need to install the Adafruit Neopixel library which can be found in the library manager.

This project also uses the handy DS3231 Simple library:- https://github.com/sleemanj/DS3231_Simple   Please follow the instruction on installing this provided on the libraries page

Before you install this code you need to set the time on your DS3231. Once you have connected it as shown in this project and have installed the DS3231_Simple library (see above) you
 to go to  'File' >> 'Examples' >> 'DS3231_Simple' >> 'Z1_TimeAndDate' >> 'SetDateTime' and follow the instructions in the example to set the date and time on your RTC

==========


 * SAY THANKS:

Buy me a coffee to say thanks: https://ko-fi.com/diymachines
Support us on Patreon: https://www.patreon.com/diymachines

SUBSCRIBE:
■ https://www.youtube.com/channel/UC3jc4X-kEq-dEDYhQ8QoYnQ?sub_confirmation=1

INSTAGRAM: https://www.instagram.com/diy_machines/?hl=en
FACEBOOK: https://www.facebook.com/diymachines/

*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#endif

#include <DS3231_Simple.h>
DS3231_Simple Clock;

// Create a variable to hold the time data
DateTime MyDateAndTime;

// Which pin on the Arduino is connected to the NeoPixels?
#define LEDCLOCK_PIN 6
#define LEDDOWNLIGHT_PIN 5

// How many NeoPixels are attached to the Arduino?
#define LEDCLOCK_COUNT 216
#define LEDDOWNLIGHT_COUNT 12

#define BRIGHTNESS_SENSOR_PIN A0

//(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
// or 3 hex byte 00 -> ff for RGB eg 0x123456 for red=12(hex) green=34(hex), and green=56(hex)
// this hex method is the same as html colour codes just with "0x" instead of "#" in front
uint32_t clockMinuteColour = 0x800000;  // pure red
uint32_t clockHourColour = 0x008000;    // pure green

int clockFaceBrightness = 0;

// Declare our NeoPixel objects:
Adafruit_NeoPixel stripClock(LEDCLOCK_COUNT, LEDCLOCK_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripDownlighter(LEDDOWNLIGHT_COUNT, LEDDOWNLIGHT_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  //Serial.begin(9600);
  Clock.begin();

  stripClock.begin();             // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripClock.show();              // Turn OFF all pixels ASAP
  stripClock.setBrightness(100);  // Set inital BRIGHTNESS (max = 255)

  stripDownlighter.begin();            // INITIALIZE NeoPixel stripClock object (REQUIRED)
  stripDownlighter.show();             // Turn OFF all pixels ASAP
  stripDownlighter.setBrightness(50);  // Set BRIGHTNESS (max = 255)
}

void loop() {
  setBrightness();
  if (clockFaceBrightness == 0) {
    stripClock.show();
    return;
  }
  //read the time
  readTheTime();

  //display the time on the LEDs
  displayTheTime();



  stripClock.show();

  //(red * 65536) + (green * 256) + blue ->for 32-bit merged colour value so 16777215 equals white
  stripDownlighter.fill(16777215, 0, LEDDOWNLIGHT_COUNT);
  stripDownlighter.show();

  //delay(500);  //this 5 second delay to slow things down during testing
}

void setBrightness() {
  int rawSensorValue = analogRead(BRIGHTNESS_SENSOR_PIN);
  clockFaceBrightness = map(rawSensorValue, 0, 1023, 0, 255);
  stripClock.setBrightness(clockFaceBrightness);  // Set brightness value of the LEDs
  Serial.print("Mapped brightness value = ");
  Serial.println(clockFaceBrightness);
}

void readTheTime() {
  // Ask the clock for the data.
  MyDateAndTime = Clock.read();

  // And use it
  Serial.println("");
  Serial.print("Time is: ");
  Serial.print(MyDateAndTime.Hour);
  Serial.print(":");
  Serial.print(MyDateAndTime.Minute);
  Serial.print(":");
  Serial.println(MyDateAndTime.Second);
  Serial.print("Date is: ");
  Serial.print(MyDateAndTime.Month);
  Serial.print("/");
  Serial.print(MyDateAndTime.Day);
  Serial.print("/20");
  Serial.println(MyDateAndTime.Year);
}

void displayTheTime() {

  stripClock.clear();  //clear the clock face


  int firstMinuteDigit = MyDateAndTime.Minute % 10;  //work out the value of the first digit and then display it
  displayNumber(firstMinuteDigit, 0, clockMinuteColour);


  int secondMinuteDigit = floor(MyDateAndTime.Minute / 10);  //work out the value for the second digit and then display it
  displayNumber(secondMinuteDigit, 63, clockMinuteColour);


  int firstHourDigit = MyDateAndTime.Hour;  //work out the value for the third digit and then display it
  if (firstHourDigit > 12) {
    firstHourDigit = firstHourDigit - 12;
  }

  // Comment out the following three lines if you want midnight to be shown as 12:00 instead of 0:00
  //  if (firstHourDigit == 0){
  //    firstHourDigit = 12;
  //  }

  firstHourDigit = firstHourDigit % 10;
  displayNumber(firstHourDigit, 126, clockHourColour);


  int secondHourDigit = MyDateAndTime.Hour;  //work out the value for the fourth digit and then display it

  // Comment out the following three lines if you want midnight to be shwon as 12:00 instead of 0:00
  //  if (secondHourDigit == 0){
  //    secondHourDigit = 12;
  //  }

  if (secondHourDigit > 12) {
    secondHourDigit = secondHourDigit - 12;
  }
  if (secondHourDigit > 9) {
    stripClock.fill(clockHourColour, 189, 18);
  }
}

void displayNumber(int digitToDisplay, int offsetBy, uint32_t colourToUse) {
  switch (digitToDisplay) {
    case 0:
      digitZero(offsetBy, colourToUse);
      break;
    case 1:
      digitOne(offsetBy, colourToUse);
      break;
    case 2:
      digitTwo(offsetBy, colourToUse);
      break;
    case 3:
      digitThree(offsetBy, colourToUse);
      break;
    case 4:
      digitFour(offsetBy, colourToUse);
      break;
    case 5:
      digitFive(offsetBy, colourToUse);
      break;
    case 6:
      digitSix(offsetBy, colourToUse);
      break;
    case 7:
      digitSeven(offsetBy, colourToUse);
      break;
    case 8:
      digitEight(offsetBy, colourToUse);
      break;
    case 9:
      digitNine(offsetBy, colourToUse);
      break;
    default:
      break;
  }
}
