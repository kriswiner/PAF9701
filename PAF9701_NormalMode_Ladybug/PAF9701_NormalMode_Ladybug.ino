/* 
   PAF9701 8 x 8 pixel low-power IR thermal sensor
   Copyright2 2021 Tlera Corporation

   PixArt Imaging's 8 x 8 pixel IR thermal imaging sensor offers wide (60 deg) field of view, 
   low-power (2 mA) normal mode current usage, wide (-20 to 380 C) object temperature range, 
   and accurate (+/- 1 degree C) 16-bit object temperatures.

   The sketch demonstrates how to cinitialize the PAF9701 in normal run mode, configure the
   data filters and image orientation, set up the data ready interrupt, read the data and plot 
   the properly scaled data on the serial monitor and on a 160 x 128 pixel Adafruit  TFT color display.

   The sketch is intended to run using a Tlera Corporation STM32L432 Ladybug development board but just about
   any 3V3 dev board with an SPI port (for the display) and I2C port (for the PAF9701) will do.

   This sketch may be used without limitations with proper attribution

   This example code is in the public domain.
*/

#include "RTC.h"
#include "PAF9701.h"
#include <Adafruit_GFX.h>    // Core graphics library, install from Arduino IDE board manager
#include <Adafruit_ST7735.h> // Hardware-specific library, install from Arduino IDE board manager
#include "SPI.h"
#include "ColorDisplay.h"

// Ladybug STM32L432 development board connections for display
#define sclk 13  // SCLK can also use pin 14
#define mosi 11  // MOSI can also use pin 7
#define cs   10  // CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23
#define dc   5   // but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22
#define rst  4   // RST can use any pin
#define sdcs 1   // CS for SD card, can use any pin

// Ladybug pin assignments
//#define myLed       13 // red

const char        *build_date = __DATE__;   // 11 characters MMM DD YYYY
const char        *build_time = __TIME__;   // 8 characters HH:MM:SS

#define I2C_BUS    Wire               // Define the I2C bus (Wire instance) you wish to use

I2Cdev             i2c_0(&I2C_BUS);   // Instantiate the I2Cdev object and point to the desired I2C bus

bool SerialDebug = true;

uint8_t seconds, minutes, hours, day, month, year;
uint8_t Seconds, Minutes, Hours, Day, Month, Year;
 
volatile bool alarmFlag = false;

// Internal STM32 definitions
float VDDA, VBUS, STM32_Temperature;

//PAF9701 definitions
#define PAF9701_intPin        8    // interrupt pin active LOW 
#define PAF9701_shutdownPin   9    // shutdown pin, HIGH for standby, LOW for run mode
#define PAF9701_resetPin      3    // reset pin active LOW

// Configure the PAF9701
uint8_t runMode = normal_mode;               // choices are normal_mode, detection_mode1, detection_mode2, detection_mode3
uint8_t freq = 4;                            // data rate in Hz, default is 4 Hz, should not be faster than 10 Hz
uint32_t sampleRate = 200000 / (256 * freq); // maximum frame time is 1342 seconds, minimum ~100 ms
uint8_t imageFlip = noflipormirror;          // choices are noflipormirror, flip, mirror, flip and mirror
uint8_t imageRotate = orient0;               // choices are orient0, orient90, orient180, or orient270
uint8_t digitalFilter = movingAverage;       // choices are normalAverage, movingAverage, IIR
uint8_t frameAverage = fourFrames;           // choices are oneFrame, twoFrames, fourFrames, and eightFrames
uint8_t IIRAverage = frames0_1;              // choices are frames 0_1, frames125_875, ..., frames825_175
bool settle_en = false;                      // allow settling (~3 sec) before sensor data made available when sensor operation resumes from suspend

int16_t rawTaData = 0, calTaData = 0;
uint8_t temp = 0;
float temperatures[64];                      //Contains the calculated object temperature of each pixel in the array
float minTemp, maxTemp, tmpTemp;

volatile bool PAF9701_intFlag = false;       // Logic flag for alert signal

PAF9701 PAF9701(&i2c_0);                     // instantiate PAF9701 class


// Configure color display
uint16_t color;
uint8_t rgb, red, green, blue;

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);


void setup()
{
  /* Enable USB UART */
  Serial.begin(115200);
  Serial.blockOnOverrun(false);
  delay(100);
  Serial.println("Serial enabled!");

  // Test the rgb led, active LOW
//  pinMode(myLed, OUTPUT);
//  digitalWrite(myLed, LOW);   // start with led off, active HIGH

  pinMode(PAF9701_shutdownPin, OUTPUT);
  digitalWrite(PAF9701_shutdownPin, LOW); // shutdown active HIGH

  pinMode(PAF9701_resetPin, OUTPUT);
  digitalWrite(PAF9701_resetPin, HIGH); // shutdown active LOW

  pinMode(PAF9701_intPin, INPUT);       // define PAF9701 interrupt
  
  pinMode(sdcs, INPUT_PULLUP);          // don't touch the SD card

  tft.initR(INITR_BLACKTAB);            // initialize a ST7735S chip, black tab
  
  //tft.begin();                        // initialize a ST7735S chip, black tab
  tft.setRotation(3);                   // 0, 2 are portrait mode, 1,3 are landscape mode
  Serial.println("initialize display");

  /* initialize wire bus */
  I2C_BUS.begin();                      // Set master mode, default on SDA/SCL for STM32L4
  I2C_BUS.setClock(400000);             // I2C frequency at 400 kHz
  delay(100);

  Serial.println("Scan for I2C devices:");
  i2c_0.I2Cscan();                      // should detect PAF9701 at 0x14 and BME280 at 0x77
  delay(100);
  
  /* Check internal STML082 and battery power configuration */
  VDDA = STM32.getVREF();
  STM32_Temperature = STM32.getTemperature();
  
  // Internal STM32L4 functions
  Serial.print("VDDA = "); Serial.print(VDDA, 2); Serial.println(" V");
  Serial.print("STM32L4 MCU Temperature = "); Serial.println(STM32_Temperature, 2);
  Serial.println(" "); 

  // Read the PAF9701 Chip ID register, this is a good test of communication
  Serial.println("PAF9701 thermal sensor...");
  uint16_t PAF9701_CHIPID = PAF9701.getChipID();  // Read CHIP_ID for PAF9701
  Serial.print("PAF9701 "); Serial.print("I AM 0x"); Serial.print(PAF9701_CHIPID, HEX); Serial.print(" I should be 0x"); Serial.println(0x0280, HEX);
  Serial.println(" ");
  delay(100); 

  if(PAF9701_CHIPID == 0x0280) // check if all I2C sensors with WHO_AM_I have acknowledged
  {
   Serial.println("PAF9701 is online..."); Serial.println(" ");

   PAF9701.coldReset();                            // software reset before initialization
   delay(200);                                     // wait 200 ms for reset 
      
   while( !(PAF9701.getStatus() & 0x20) ) {}       // wait for flash bootload to complete
   Serial.println("Flash Bootload done!"); Serial.println(" ");
   PAF9701.init(runMode, sampleRate, settle_en);  // select sensor run mode
   Serial.print("Sample rate = 0x"); Serial.println(sampleRate, HEX); Serial.println(" ");
   temp = PAF9701.getPowerSaveMode();
   Serial.print("power save mode register = 0x"); Serial.println(temp, HEX); 
   PAF9701.setFilter(digitalFilter, frameAverage, IIRAverage);
   PAF9701.imageOrientation(imageFlip, imageRotate);
   PAF9701.clearInterrupt();
   PAF9701.resumeOperation();
   if(settle_en) delay(3000); // takes about 3 seconds to settle when settle function enabled
  }
  else 
  {
  if(PAF9701_CHIPID != 0x0280) Serial.println(" PAF9701 not functioning!");
  }

  /* Set the RTC time */
  SetDefaultRTC();
  
  // set alarm to update the RTC periodically
//  RTC.setAlarmTime(0, 0, 0);
  RTC.enableAlarm(RTC.MATCH_ANY); // alarm once a second
  RTC.attachInterrupt(alarmMatch);

  attachInterrupt(PAF9701_intPin, PAF9701_inthandler, FALLING);  // attach  interrupt for INT pin output of PAF9701
  PAF9701.clearInterrupt();
} /* end of setup */

void loop()
{
  // PAF9701 interrupt handling
  if(PAF9701_intFlag) { // data ready
     PAF9701_intFlag = false;

  PAF9701.clearInterrupt();

  // Get PAF9701 data
  rawTaData = PAF9701.getRawTaData(); // ambient temperature
  calTaData = PAF9701.getCalTaData();
  PAF9701.getToData(temperatures);    // object temperature

  // Get min and max temperatures for display
  minTemp = 1000.0f;
  maxTemp =    0.0f;
  for(int y=0; y<8; y++){ //go through all the rows
  for(int x=0; x<8; x++){ //go through all the columns
    if(temperatures[y+x*8] > maxTemp) maxTemp = temperatures[y+x*8];
    if(temperatures[y+x*8] < minTemp) minTemp = temperatures[y+x*8];
    }
    }

  for(int y=0; y<8; y++){ //go through all the rows
  for(int x=0; x<8; x++){ //go through all the columns
    tmpTemp = temperatures[y+x*8];
    Serial.print(tmpTemp, 1); Serial.print(","); // use the serial monitor to plot the data, TFT diplay would be better
    if(x == 7) Serial.println(" "); 
    if(y+x*8 == 63) Serial.println(" "); 

    rgb =(uint8_t) (((tmpTemp - minTemp)/(maxTemp - minTemp)) * 199); // 0 - 199 = 200 possible rgb color values

    red   = rgb_colors[rgb*3] >> 3;          // keep 5 MS bits
    green = rgb_colors[rgb*3 + 1] >> 2;      // keep 6 MS bits
    blue  = rgb_colors[rgb*3 + 2] >> 3;      // keep 5 MS bits
    color = red << 11 | green << 5 | blue;   // construct rgb565 color for tft display

    tft.fillRect(x*16, y*16, 16, 16, color); // data on 128 x 128 pixels of a 160 x 128 pixel display
  }
  }
    tft.fillRect(128, 0, 32, 128, BLACK);    // fill 32 x 128 pixel non-data patch with black

    tft.setRotation(0);                      // 0, 2 are portrait mode, 1,3 are landscape mode
    tft.setTextSize(0);
    tft.setTextColor(WHITE);
    tft.setCursor(32, 4 );                   // write min,max temperature on non-data patch
    tft.print("min T = "); tft.print((uint8_t) minTemp); tft.print(" C");
    tft.setCursor(32, 20 );
    tft.print("max T = "); tft.print((uint8_t) maxTemp); tft.print(" C");
    tft.setRotation(3);                      // 0, 2 are portrait mode, 1,3 are landscape mode

    if(SerialDebug) {
      Serial.print("min T = "); Serial.println((uint8_t) minTemp);
      Serial.print("max T = "); Serial.println((uint8_t) maxTemp);
    }  
  } /* end of PAF9701 interrupt handling

 
  /*RTC*/
  if (alarmFlag) { // update RTC output at the alarm
      alarmFlag = false;

  // output some data from the PAF9701
    if(SerialDebug) {
      Serial.print("Raw Ta ADC counts = "); Serial.println(rawTaData);  
      Serial.print("Cal Ta Data = "); Serial.print((float)calTaData * 0.03125f); Serial.println(" C"); Serial.println(" ");
    }
    
  VDDA = STM32.getVREF();
  STM32_Temperature = STM32.getTemperature();
    if(SerialDebug) {
      Serial.print("VDDA = "); Serial.print(VDDA, 2); Serial.println(" V");
      Serial.print("STM32L4 MCU Temperature = "); Serial.println(STM32_Temperature, 2);
      Serial.println(" ");
    }

  Serial.println("RTC:");
  Day = RTC.getDay();
  Month = RTC.getMonth();
  Year = RTC.getYear();
  Seconds = RTC.getSeconds();
  Minutes = RTC.getMinutes();
  Hours   = RTC.getHours();     
  if(Hours < 10) {Serial.print("0"); Serial.print(Hours);} else Serial.print(Hours);
  Serial.print(":"); 
  if(Minutes < 10) {Serial.print("0"); Serial.print(Minutes);} else Serial.print(Minutes); 
  Serial.print(":"); 
  if(Seconds < 10) {Serial.print("0"); Serial.println(Seconds);} else Serial.println(Seconds);  

  Serial.print(Month); Serial.print("/"); Serial.print(Day); Serial.print("/"); Serial.println(Year);
  Serial.println(" ");
  
//  digitalWrite(myLed, HIGH); delay(1);  digitalWrite(myLed, LOW); // toggle blue led on
 } /* end of RTC alarm section */
 
    
//    STM32.stop();        // Enter STOP mode and wait for an interrupt
    STM32.sleep();        // Enter SLEEP mode and wait for an interrupt
   
}  /* end of loop*/


/* Useful functions */
void PAF9701_inthandler()
{
  PAF9701_intFlag = true; 
}


void alarmMatch()
{
  alarmFlag = true;
}


void SetDefaultRTC()                                                                                 // Function sets the RTC to the FW build date-time...
{
  char Build_mo[3];
  String build_mo = "";

  Build_mo[0] = build_date[0];                                                                       // Convert month string to integer
  Build_mo[1] = build_date[1];
  Build_mo[2] = build_date[2];
  for(uint8_t i=0; i<3; i++)
  {
    build_mo += Build_mo[i];
  }
  if(build_mo == "Jan")
  {
    month = 1;
  } else if(build_mo == "Feb")
  {
    month = 2;
  } else if(build_mo == "Mar")
  {
    month = 3;
  } else if(build_mo == "Apr")
  {
    month = 4;
  } else if(build_mo == "May")
  {
    month = 5;
  } else if(build_mo == "Jun")
  {
    month = 6;
  } else if(build_mo == "Jul")
  {
    month = 7;
  } else if(build_mo == "Aug")
  {
    month = 8;
  } else if(build_mo == "Sep")
  {
    month = 9;
  } else if(build_mo == "Oct")
  {
    month = 10;
  } else if(build_mo == "Nov")
  {
    month = 11;
  } else if(build_mo == "Dec")
  {
    month = 12;
  } else
  {
    month = 1;                                                                                       // Default to January if something goes wrong...
  }
  if(build_date[4] != 32)                                                                            // If the first digit of the date string is not a space
  {
    day   = (build_date[4] - 48)*10 + build_date[5]  - 48;                                           // Convert ASCII strings to integers; ASCII "0" = 48
  } else
  {
    day   = build_date[5]  - 48;
  }
  year    = (build_date[9] - 48)*10 + build_date[10] - 48;
  hours   = (build_time[0] - 48)*10 + build_time[1]  - 48;
  minutes = (build_time[3] - 48)*10 + build_time[4]  - 48;
  seconds = (build_time[6] - 48)*10 + build_time[7]  - 48;
  RTC.setDay(day);                                                                                   // Set the date/time
  RTC.setMonth(month);
  RTC.setYear(year);
  RTC.setHours(hours);
  RTC.setMinutes(minutes);
  RTC.setSeconds(seconds);
}
