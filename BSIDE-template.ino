
// This is the master template for Arduino IDE development on the
// BSIDES Canberra 2016 "badge" development board.
// Badge design by Ingmar M. Dec2015-Apr2016 (c) until waived by author.

// Due to the restricted number of I/O lines available on the ESP8266
// limitations exist in what peripherals can be used simultaneously.
// Conflicts exist between TXD0/RXD0 - which is the standard serial port
// connected to the CH340 USB/Serial bridge - and PBDOWN and ADEXP which
// means you can either use the USB serial port OR PBDOWN and ADEXP chip.

#include <SPI.h>              // SPI library used for TFT, SDCARD, GPIO expander and A/D expansion.
#include <MCP23S17.h>         // GPIO expansion
#include <SD.h>               // SDCARD library
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
//
#include <WiFiServer.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//

// These defines enable or disable init and startup code for the optional peripherals.
#undef  USESD
#undef  USEAD
#define USEIO
#define USESERIAL

// Define I/O used by all peripherals.
#define ADEXP   1  // Active LOW to CS MCP2308 8 input A/D
#define IOEXP   5  // Active LOW to CS MCP23S17 SPI I/O expander
#define PBSEL   0  // Active LOW from SELECT button
#define PBUP    0  // Shared with VBatt analogue input 0 = button pushed
#define VBATT   0  // Shared with PBUP
#define PBDOWN  3  // Active LOW from DOWN button, line SHARED with RXD0 serial input pin.
#define TFTDC   4  // D/C select line to TFT
#define TFTCS   15 // Active LOW to TFT
#define TFTBL   16 // Active HIGH to turn on TFT backlight
#define TFTRST  0  // No pin assigned to TFT reset
#define SDCS    2  // Active LOW to SD card CS,

// Create instances of our peripherals.

// TFT display based on ST7735 controller.
Adafruit_ST7735 tft = Adafruit_ST7735(TFTCS, TFTDC, TFTRST);

// SD utility library functions:
#ifdef USESD
Sd2Card sdcard;
SdVolume sdvolume;
SdFile sdroot;
#endif

#ifdef USEIO
  MCP gpio(0, IOEXP); // config the MCP23S17 GPIO chip as address 0 on CS line IOEXP
#endif

void setup() {
  // I/O declarations
  
  pinMode(PBSEL, INPUT_PULLUP); // input with internal pullup resistor.
  pinMode(PBUP, INPUT_PULLUP); // input with internal pullup rsistor.
  pinMode(TFTDC, OUTPUT);
  pinMode(TFTCS, OUTPUT);  
  pinMode(TFTBL, OUTPUT);
  pinMode(IOEXP, OUTPUT);
  pinMode(SDCS, OUTPUT);
  // pinMode(ADEXP, OUTPUT); // only if serial 0 TXD is NOT used.
  // pinMode(PBDOWN, INPUT_PULLUP); // only if serial 0 RXD is not used.
  
  // Peripheral setup code
  digitalWrite(SDCS, HIGH); // SD disabled
  digitalWrite(IOEXP, HIGH); // GPIO disabled
  analogWrite(TFTBL, 512);  // TFT backlight 50%
#ifdef USEAD
  digitalWrite(ADEXP, HIGH); // AD disabled
#endif

#ifdef USESERIAL
  Serial.begin(9600);
  Serial.println("Serial port enabled");
#endif

#ifdef USEIO
  // Setup which pins are input and output. Eg follows.
  gpio.pinMode(1, OUTPUT); // port GPA0 output
  gpio.digitalWrite(1, LOW);  // set the pin low
#endif

  // Turn off wifi unless you are using it! 
  WiFi.mode(WIFI_OFF);
  
  // Initialise the TFT etc.
  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
  tft.setRotation(1); // Landscape mode across the badge
  tft.fillScreen(ST7735_BLACK); // clear the screen
  
#ifdef USESD  
  if (! sdcard.init(SPI_FULL_SPEED, SDCS))
  {
#ifdef USESERIAL
    Serial.println("SD not initialised.");
#endif
  } else {
#ifdef USESERIAL
    Serial.println("SD initialised");
#endif
  }

#endif
}

void loop() {
  // put your main code here, to run repeatedly:
  //IDent();
  boolean rc = false;
  
  gpio.digitalWrite(1, HIGH); // Test gpio pin.

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  //tft.fillScreen(ST7735_BLACK);
  //tft.setCursor(0,0);
  //tft.setTextColor(ST7735_YELLOW);
  //tft.setTextSize(1);
  //tft.print("Scanning..");
  
  int nets = WiFi.scanNetworks(); // scan for access points

  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(ST7735_YELLOW);
  tft.print("Found:");
  tft.println(nets);

  tft.setTextColor(ST7735_WHITE);
  rc = false;
  
  for (int s=0; s<nets; s++) {
    if (rc == true) {
      rc = false;
      tft.setTextColor(ST7735_WHITE);
    } else {
      rc = true;
      tft.setTextColor(ST7735_CYAN);
    }
    
    tft.print(WiFi.SSID(s));
    tft.print(" ");
    tft.print(WiFi.RSSI(s));
    tft.println("dBm ");

    /*uint8_t  * bssid = WiFi.BSSID(s);
    
    tft.print(bssid[0], HEX);
    tft.print(bssid[1], HEX);
    tft.print(bssid[2], HEX);
    tft.print(bssid[3], HEX);
    tft.print(bssid[4], HEX);
    tft.println(bssid[5], HEX);*/
  }

  gpio.digitalWrite(1 , LOW); 
  delay(3000);
}

void IDent() {
  const int T = 180;
  const char IDENT[] = {0xab, 0x8e, 0xb8, 0xbb, 0xbb, 0x8e, 0xa8, 0xee, 0x8e};
  const int BITS = 9 * 8;

  int i,j;

  digitalWrite(TFTBL, LOW);
  delay(1000);
  for (i=0; i<(BITS / 8); i++) { 
    for (j=7; j>=0; j--) {
      if ( (IDENT[i] >> j) & 0x01 ) {
        digitalWrite(TFTBL, HIGH);
        delay(T);
      } else {
        digitalWrite(TFTBL, LOW);
        delay(T);
      }
    } // j
  } // i
  
}

// Return VIN voltage scaled so that 740 = 4.55V ie 4.55/740=0.00615v per unit.
// This is with a 5k6 to ground and a 5k6 in series with the input to the
// NodeMCU modules already scaled input on A0.

int getVbatt() {
  int aVal;

  aVal = analogRead(A0); // read scaled input from VIN line.

  if (aVal <=1) // This is so we get a real 0 if the PBUP button is pressed.
    return 0;
  else
  return aVal; // 0-1023
}

// Return VIN as floating point volts.
double getVIN() {
  return getVbatt() * 0.00615;
}

void graphV() {
  double gscale = (1023 * 0.00615) / 160; // gives scaling factor for X axis
  int vx;
  
  //tft.fillRect(0,0,159,10, ST7735_BLACK);
  tft.drawFastHLine(0,0,159,ST7735_BLACK);
  tft.drawFastHLine(0,1,159,ST7735_BLACK);
  tft.drawFastHLine(0,2,159,ST7735_BLACK);
  tft.drawFastHLine(0,3,159,ST7735_BLACK);
  tft.drawFastHLine(0,4,159,ST7735_BLACK);

  vx = int(getVIN() / gscale);

  tft.drawFastHLine(0,0,vx,ST7735_GREEN);
  tft.drawFastHLine(0,1,vx,ST7735_GREEN);
  tft.drawFastHLine(0,2,vx,ST7735_GREEN);
  tft.drawFastHLine(0,3,vx,ST7735_GREEN);
  tft.drawFastHLine(0,4,vx,ST7735_GREEN);

  //tft.fillRect(0,0,vx,10, ST7735_GREEN);
}

