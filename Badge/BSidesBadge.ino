
// This is the master template for Arduino IDE development on the
// BSIDES Canberra 2016 "badge" development board.
// Badge design by Ingmar M. Dec2015-Apr2016 (c) until waived by author.
//
// Code by Silvio Cesare.
//
// Due to the restricted number of I/O lines available on the ESP8266
// limitations exist in what peripherals can be used simultaneously.
// Conflicts exist between TXD0/RXD0 - which is the standard serial port
// connected to the CH340 USB/Serial bridge - and PBDOWN and ADEXP which
// means you can either use the USB serial port OR PBDOWN and ADEXP chip.

#include <SPI.h>              // SPI library used for TFT, SDCARD, GPIO expander and A/D expansion.
#include <MCP23S17.h>         // GPIO expansion
//#include <SD.h>               // SDCARD library
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library
//
#include <WiFiServer.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include "TrigTables.h"
#include "BSidesLogo3.h"

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


struct schedule_s {
  const char *slot;
  const char *talk;
  const char *speaker;
};

struct schedule_s day1schedule[] = {
  { " 9:00am -  9:10am", "Conference Opening", "Silvio & Kylie" },
  { " 9:10am - 10:00am", "Keynote - My Cyber is Trickling Down and the other Diverse Problems of the Aging Hacker", "Metlstorm" },
  { "10:00am - 10:50am", "Catching 'Rays", "Steve Glass" },
  { "10:50am - 11:10am", "VoWIFI and you! - how someone could have read your text messages", "David" },
  { "11:10am - 12:00  ", "Scrutiny on the Bounty (pun hall of fame plz)", "Nathaniel & Shubs" },
  { "12:00   -  1:00pm", "LUNCH BREAK", "" },
  { " 1:00pm -  1:50pm", "Building Challenge Locks", "Klepas" },
  { " 1:50pm -  2:10pm", "Open Sesame - bypassing BMCs Tradecraft", "Dan Kennedy" },
  { " 2:10pm -  3:00pm", "The Road to Shell is Paved with Good Intentions", "Liam & Wily" },
  { " 3:00pm -  3:20pm", "AFTERNOON BREAK", "" },
  { " 3:20pm -  4:10pm", "Hacking Fibre Channel (FC) Networks", "Kylie" },
  { " 4:10pm -  4:30pm", "Threat Landscape Gardening", "Neal Wise" },
  { " 4:30pm -  5:20pm", "Adventures in Glitching PIC Microcontrollers to Defeat Firmware Copy Protection", "Dr Silvio Cesare" },
  { " 5:20pm -  5:30pm", "Day Close", "Silvio & Kylie" },
  { NULL, NULL, NULL }
};

struct schedule_s day2schedule[] = {
  { " 9:00am -  9:10am", "Day Two Welcome Back", "Silvio & Kylie" },
  { " 9:10am - 10:00am", "Attacking High Security Lock Systems", "Topy" },
  { "10:00am - 10:20am", "Countering Cyber Adversary Tradecraft", "Matt Wilcox" },
  { "10:20am - 11:10am", "Global Honeypot Trends", "Elliot Brink" },
  { "11:10am - 12:00  ", "Naval-gazing with Docker", "Frenchie" },
  { "12:00   -  1:00pm", "LUNCH BREAK" },
  { " 1:00pm -  1:50pm", "EFF 'Secure' IM Scorecard Review", "Dan & Matt" },
  { " 1:50pm -  2:10pm", "Die Hard 7 - Passwords Please" },
  { " 2:10pm -  3:00pm", "All your Bases are Belong to Us, the first 2^32 Years of Security", "Pearce & Stewart" },
  { " 3:00pm -  3:20pm", "AFTERNOON BREAK", "" },
  { " 3:20pm -  4:10pm", "Why you shouldn't use SSL" },
  { " 4:10pm -  4:30pm", "Elevator Basics & Potential Future Vulnerabilities", "Wizzy (aka Josh)" },
  { " 4:30pm -  5:20pm", "Active Incident Response", "Candlish & Teutenberg" },
  { " 5:20pm -  5:40pm", "Awards & Closing Ceremony", "Silvio & Kylie" },
  { NULL, NULL, NULL }
};

struct vertex_s {
  float v[3];
  int x, y;
};

struct edge_s {
  int v1, v2;
};

struct matrix_s {
  float v[3][3];
};

#define NUMVERTICES 8

struct vertex_s boxVertices[NUMVERTICES] = {
  { { -1.0, -1.0, -1.0 }, 0, 0 },
  { {  1.0, -1.0, -1.0 }, 0, 0 },
  { {  1.0,  1.0, -1.0 }, 0, 0 },
  { { -1.0,  1.0, -1.0 }, 0, 0 },
  { { -1.0, -1.0,  1.0 }, 0, 0 },
  { {  1.0, -1.0,  1.0 }, 0, 0 },
  { {  1.0,  1.0,  1.0 }, 0, 0 },
  { { -1.0,  1.0,  1.0 }, 0, 0 }
};

struct vertex_s transformedVertices[NUMVERTICES];

#define NUMEDGES 12

struct edge_s boxEdges[NUMEDGES] = {
  { 0, 1 },
  { 1, 2 },
  { 2, 3 },
  { 3, 0 },

  { 4, 5 },
  { 5, 6 },
  { 6, 7 },
  { 7, 4 },

  { 0, 4 },
  { 1, 5 },
  { 2, 6 },
  { 3, 7 }
};

uint16_t staticTextColor = 0;

const char msg8[] = "BSides is OLD SCH00L";
const char welcomeMessage[] = "Welcome to BSides Canberra - Kylie & Silvio!";

#define MSGBUFSZ 250
#define REALTIMEMSGSZ 250

#define USEWIFI

WiFiClient client;
bool wifiConnected = false;
char msgbuf[MSGBUFSZ + 1];
char realtimeMsg[REALTIMEMSGSZ + 1];
 
void setup() {
  // I/O declarations

  WiFi.persistent(false);
  
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
  analogWrite(TFTBL, 150);  // TFT backlight
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

#ifdef USEWIFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
#endif

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

  connectWifi();
}

bool firstTime = true;

void loop() {
  if (firstTime) {
    while (drawCustom() == 0);
    firstTime = false;
  } else {
    drawCustom();
  }
  drawLogo();
  drawMessageScroller(12, 200);
  drawSchedule();
  drawNetworkScroller(12, "Live Twitter Stream", 200);
  draw8(msg8, 10); 
  drawRotatingBox("BSides Canberra", 4);
  drawCredits();
}

void
drawLogo()
{
  tft.fillScreen(ST7735_BLACK);
  bmpDraw(15, 10);
  for (int i = 0; i < 300; i++) {
    if (checkButtonSelect())
      return;
    delay(10);
  }
}

void
drawCredits()
{
  tft.fillScreen(ST7735_BLACK);
  testroundrects();
  for (int i = 0; i < 200; i++) {
    tft.setCursor(0,0);
    tft.setTextSize(2);
    tft.setTextColor(staticTextColor += 300);
    tft.println("Badge Credits");

    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);

    tft.println();
    tft.println();
    tft.println();
    
    tft.println("  Badge designed by Iggy");
    tft.println("  Code by Silvio");
    tft.println("  Network by Kylie");
    tft.println("  Sponsored by Telstra");
    tft.println("  Assembly by AAron,");
    tft.println("    Andrew, Declan,");
    tft.println("    George, Klepas,");
    tft.println("    Nathan, Paul, Peter");

    if (checkButtonSelect())
      return;

    delay(10);
  }

}

enum http_state_e {
    HTTP_RESPONSE_HDR,
    HTTP_RESPONSE_NEWLINE,
    HTTP_RESPONSE_DATA
};

//#define SPEAKER
#define DELEGATE
//#define STAFF
//#define HEADHONCHO

#ifdef SPEAKER
char customMsg[] = "SPEAKER";
uint16_t customColour = ST7735_YELLOW;
#endif

#ifdef DELEGATE
char customMsg[] = "DELEGATE";
uint16_t customColour = ST7735_WHITE;
#endif

#ifdef STAFF
char customMsg[] = "Nathan";
uint16_t customColour = ST7735_RED;
#endif

#ifdef HEADHONCHO
char customMsg[] = "Silvio";
uint16_t customColour = ST7735_RED;
#endif

int
drawCustom(void)
{
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(12);
  tft.setTextColor(customColour);
  for (char *p = customMsg; *p; p++) {
    if (checkButtonSelect())
      return 1;
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(50,20);
    tft.print(*p);
    tft.invertDisplay(false);    
    for (int i = 0; i < 20; i++) {
#ifdef HEADHONCHO
      tft.fillScreen(customColour + 10000);
      tft.setCursor(50,20);
      tft.setTextColor(customColour);
      tft.print(*p);
      customColour += 5000;
#endif
      if (checkButtonSelect())
        return 1;
      delay(10);
    }
    tft.invertDisplay(true);    
    for (int i = 0; i < 20; i++) {
      if (checkButtonSelect())
        return 1;
      delay(10);
    }
    for (int i = 0; i < 5; i++) {
      if (checkButtonSelect())
        return 1;
      delay(10);
    }
    tft.invertDisplay(false);
  }
  return 0;
}

void
drawMessageScroller(int w, int n)
{
  if (!wifiConnected) {
    connectWifi();
  }
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(1);
  if (getNetworkMsg(realtimeMsg, REALTIMEMSGSZ, "/BSidesMessages.txt") > 0) {
    drawYScroller(12, realtimeMsg, "Realtime Updates", 100);
  } else {
    drawYScroller(12, welcomeMessage, "Realtime updates", 100);
  }
}

void
makeScaleMatrix(struct matrix_s *r, float x, float y, float z)
{
  r->v[0][0] = x;    r->v[0][1] = 0.0;  r->v[0][2] = 0.0;  
  r->v[1][0] = 0.0;  r->v[1][1] = y;    r->v[1][2] = 0.0;  
  r->v[2][0] = 0.0;  r->v[2][1] = 0.0;  r->v[2][2] = z;  
}

void
matrixMultiply(struct matrix_s *r, struct matrix_s *x, struct matrix_s *y)
{
  for (int i = 0; i < 3; i++) { // row
    for (int j = 0; j < 3; j++) { // column
        r->v[i][j] = 0.0;
      for (int k = 0; k < 3; k++) {
        r->v[i][j] += x->v[i][k] * y->v[k][j];
      }
    }
  }
}

void
makeRotateXMatrix(struct matrix_s *r, int angle)
{
  r->v[0][0] = 1.0; r->v[0][1] = 0.0;                 r->v[0][2] = 0.0;
  r->v[1][0] = 0.0; r->v[1][1] = cosineTable[angle];  r->v[1][2] = -sineTable[angle];
  r->v[2][0] = 0.0; r->v[2][1] = sineTable[angle];    r->v[2][2] = cosineTable[angle];
}

void
makeRotateYMatrix(struct matrix_s *r, int angle)
{
  r->v[0][0] = cosineTable[angle]; r->v[0][1] = 0.0;  r->v[0][2] = -sineTable[angle];
  r->v[1][0] = 0.0;                r->v[1][1] = 1.0;  r->v[1][2] = 0.0;
  r->v[2][0] = sineTable[angle];  r->v[2][1] = 0.0;  r->v[2][2] = cosineTable[angle];
}

void
makeRotateZMatrix(struct matrix_s *r, int angle)
{
  r->v[0][0] = cosineTable[angle]; r->v[0][1] = -sineTable[angle];  r->v[0][2] = 0.0;
  r->v[1][0] = sineTable[angle];   r->v[1][1] = cosineTable[angle]; r->v[1][2] = 0.0;
  r->v[2][0] = 0.0;                r->v[2][1] = 0.0;                r->v[2][2] = 1.0;
}

void
matrixVectorMultiply(struct vertex_s *r, const struct vertex_s *v, struct matrix_s *x)
{
  for (int i = 0; i < 3; i++) {
    r->v[i] = 0.0;
    for (int j = 0; j < 3; j++) {
      r->v[i] += v->v[j] * x->v[i][j];
    }
  }
}

#define translateVectorX(V, TX) do { ((V)->v[0] += (TX)); } while (0)
#define translateVectorY(V, TY) do { ((V)->v[1] += (TY)); } while (0)
#define translateVectorZ(V, TZ) do { ((V)->v[2] += (TZ)); } while (0)

void
perspectiveView(struct vertex_s *v)
{
  if (v->v[2] > 0) {
    v->x = 60.0*(v->v[0] / (v->v[2])) + 80;
    v->y = 60.0*(v->v[1] / (v->v[2])) + 60;
  }
}

void
drawRotatingBox(const char *staticText, int n)
{
  for (int count = 0; count < n; count++) {
    tft.fillScreen(ST7735_BLACK);
    
    for (int angle = 0; angle < 256; angle += 4) {
      struct matrix_s transformationMatrixX, transformationMatrixY, transformationMatrixZ;
      struct matrix_s transformationMatrixTemp1, transformationMatrixTemp2, transformationMatrix;
      struct matrix_s transformationMatrixScale;

      if (checkButtonSelect())
        return;

      tft.setCursor(0, 0);
      tft.setTextColor(staticTextColor);
      staticTextColor += 100;
      tft.setTextSize(1);
      tft.print(staticText);

      makeRotateXMatrix(&transformationMatrixX, angle);
      makeRotateYMatrix(&transformationMatrixY, angle);
      makeRotateZMatrix(&transformationMatrixZ, angle);
      makeScaleMatrix(&transformationMatrixScale, 60, 60, 60);
      
      matrixMultiply(&transformationMatrixTemp1, &transformationMatrixX, &transformationMatrixY);
      matrixMultiply(&transformationMatrixTemp2, &transformationMatrixTemp1, &transformationMatrixZ);
      matrixMultiply(&transformationMatrix, &transformationMatrixTemp1, &transformationMatrixScale);

      for (int i = 0; i < NUMVERTICES; i++) {
        translateVectorX(&transformedVertices[i], 2);
        matrixVectorMultiply(&transformedVertices[i], &boxVertices[i], &transformationMatrix);
        translateVectorZ(&transformedVertices[i], 200);
        perspectiveView(&transformedVertices[i]);
      }
      for (int i = 0; i < NUMEDGES; i++) {
        tft.drawLine(
          transformedVertices[boxEdges[i].v1].x, transformedVertices[boxEdges[i].v1].y,
          transformedVertices[boxEdges[i].v2].x, transformedVertices[boxEdges[i].v2].y,
          ST7735_WHITE);
      }
      delay(100);
      for (int i = 0; i < NUMEDGES; i++) {
        tft.drawLine(
          transformedVertices[boxEdges[i].v1].x, transformedVertices[boxEdges[i].v1].y,
          transformedVertices[boxEdges[i].v2].x, transformedVertices[boxEdges[i].v2].y,
          ST7735_BLACK);
      }
    }
  }
}


int
checkButtonSelect()
{
  if (digitalRead(PBSEL) == LOW) {
    tft.invertDisplay(true);
    delay(1000);
    tft.invertDisplay(false);
    return 1;
  }
  return 0;
}

void
drawSchedule(void)
{
  drawSchedule("Day 1", day1schedule);
  drawSchedule("Day 2", day2schedule);
}
 
void
draw8(const char *msg, int n)
{
  int len;
  int t;
  int index;
  int displacement;
  int count;
  int deltaT = 8;

  tft.fillScreen(ST7735_BLACK);
  drawCopper(10, 4, 0);
  drawCopper(100, 4, 0);
  tft.setTextSize(1);
  displacement = 0;
  index = 0;
  len = strlen(msg);
  for (displacement = 0; displacement < n*256;) {
    tft.setTextColor(ST7735_BLACK);
    index = 0;
    for (t = 0; t < 256; t += deltaT) {
      int realT;

#define rescaleX(X) (int)(((X)+1)*50.0+20.0)
#define rescaleY(Y) (int)(((Y)+1)*40.0+20.0)

      if (checkButtonSelect())
        return;
        
      realT = (displacement + t) % 256;
      tft.setCursor(rescaleX(cosineTable[realT]),
        rescaleY(cosineTable[realT] * sineTable[realT]));
      if (msg[index])
        tft.print(msg[index]);
      index++;
      if (index > len)
        break;
    }
    displacement++;
    tft.setTextColor(ST7735_RED);
    index = 0;
    for (t = 0; t < 256; t += deltaT) {
      int realT;

      if (checkButtonSelect())
        return;

      realT = (displacement + t) % 256;
      tft.setCursor(rescaleX(cosineTable[realT]),
        rescaleY(cosineTable[realT] * sineTable[realT]));
      if (msg[index])
        tft.print(msg[index]);
      index++;
      if (index > len)
        break;
    }
    delay(20);
  }
}

void
connectWifi(void)
{
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0,20);
  tft.setTextSize(1);
#ifdef USEWIFI
  if (!wifiConnected) {
    int i;

#define PUBLIC_SSID  "BSidesCTF"
#ifdef PUBLIC_SSID
    int s, nets;
    nets = WiFi.scanNetworks();
    for (s = 0; s < nets; s++) {
      if (strcmp(WiFi.SSID(s).c_str(), PUBLIC_SSID) == 0)
        break;
    }
    if (s == nets)
      return;
#endif

    tft.println("Connecting to WiFi...");
    
    WiFi.begin("BSidesBadge","xxxchangeme-justkiddingnub*&#$");
    i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 10) delay(1000);
  
    if(i == 11) {
      tft.println("Could not connect.."); 
      delay(1000);
    } else {
      tft.println("Connected to wifi");
      delay(1000);
      wifiConnected = true; 
    }
  }
#endif
}

int
getNetworkMsg(char *buf, int bufSz, const char *path)
{
  if (!wifiConnected) {
    connectWifi();
  }
  if (wifiConnected) {
    int i;
    enum http_state_e state = HTTP_RESPONSE_HDR;
      
    client.connect("172.16.255.254", 80);
    client.print("GET ");
    client.print(path);
    client.print(" HTTP/1.1 \r\n");
    client.print("Host: 172.16.255.254\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n");
    
    i = 0;
    delay(10);
    while (client.available() && i < bufSz) {
      char c = client.read();

      if (state != HTTP_RESPONSE_DATA ) {
        switch(state) {
          case HTTP_RESPONSE_HDR:
            if (c == '\n') state = HTTP_RESPONSE_NEWLINE;
            break;
          
          case HTTP_RESPONSE_NEWLINE:
           if (c == '\r' || c == '\n') state = HTTP_RESPONSE_DATA;
            else state = HTTP_RESPONSE_HDR;
            break;
            
          default:
            ;
        }
      } else {
        buf[i++] = c;
      }
    }
    buf[i] = 0;
    return i;
  }
  return 0;
}

void
drawYScrollerLine(int n, int offsetX, int offsetY, const char *text, int start, int len)
{
  int i;

  for (i = 0; i < n; i++) {
    char c;

    c = text[(start + i) % len];
    tft.setCursor(offsetX,
      (int)(((sineTable[offsetY % 256]+1.0)*18.0+40)));
    if (c == '\r' || c == '\n')
      tft.print(' ');
    else
      tft.print(c);
    offsetX += 12;
    offsetY += 2;
  }
}

void
drawYScroller(int n, const char *text, const char *staticText, int timeToRun)
{
  int len;
  int i, j;
  int index;
  int offsetX;
  int charWidth = 12;
  int offsetY = 0;
  
  offsetX = charWidth;
  index = 0;
  len = strlen(text);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(2);
  drawCopper(5, 2, 5);
  drawCopper(90, 2, 5);
  for (i = 0; i < timeToRun*charWidth; i++) {
    if (checkButtonSelect())
        return;

    tft.setCursor(0, 0);
    tft.setTextColor(staticTextColor);
    staticTextColor += 100;
    tft.setTextSize(1);
    tft.print(staticText);
    
    tft.setTextSize(2);
    tft.setTextColor(ST7735_BLACK);
    drawYScrollerLine(n, offsetX, offsetY, text, (index + 0) % len, len);
    offsetX--;
    if (offsetX == 0) {
      offsetX = charWidth;
      index = (index + 1) % len;
    }
    offsetY++;
    tft.setTextColor(ST7735_YELLOW);
    drawYScrollerLine(n, offsetX, offsetY, text, (index + 0) % len, len);
    delay(10);
  }
}

void
drawScrollerLine(int n, const char *text, int start, int len)
{
  int i;

  tft.setTextSize(2);
  for (i = 0; i < n; i++) {
    char c;

    c = text[(start + i) % len];
    if (c == '\r' || c == '\n')
      tft.print(' ');
    else
      tft.print(c);
  }
}

void
drawNetworkScroller(int w, const char *staticText, int timeToRun)
{
  if (getNetworkMsg(msgbuf, MSGBUFSZ, "/BSidesTwitterStream.txt") > 0) {
    drawScroller(w, msgbuf, staticText, timeToRun);
  }
}
  
void
drawScroller(int n, const char *text, const char *staticText, int timeToRun)
{
  int len;
  int i, j;
  int index;
  int offsetX;
  int charWidth = 12;
  int offsetY = 0;
  
  offsetX = charWidth;
  index = 0;
  len = strlen(text);
  tft.fillScreen(ST7735_BLACK);
  for (i = 0; i < timeToRun*charWidth; i++) {
    if (checkButtonSelect())
        return;

    if ((i % 2) == 0) {
      int realY;   

      realY = (int)((sineTable[offsetY % 256] + 1.0)*10.0);
      deleteCopper(10 + realY, 4);
      deleteCopper(70 + realY, 4);

      offsetY += 16;

      realY = (int)((sineTable[offsetY % 256] + 1.0)*10.0);
      drawCopper(10 + realY, 4, 11);
      drawCopper(70 + realY, 4, 11);
    }

    tft.setCursor(0, 0);
    tft.setTextColor(staticTextColor);
    staticTextColor += 100;
    tft.setTextSize(1);
    tft.print(staticText);

    tft.setTextSize(2);
    tft.setCursor(offsetX, 50);
    tft.setTextColor(ST7735_BLACK);
    drawScrollerLine(n, text, (index + 0) % len, len);
    offsetX -= 2;
    if (offsetX == 0) {
      offsetX = charWidth;
      index = (index + 1) % len;
    }
    tft.setCursor(offsetX, 50);
    tft.setTextColor(ST7735_YELLOW);
    drawScrollerLine(n, text, (index + 0) % len, len);
    delay(5);
  }
}

void
deleteCopper(int startY, int scale)
{
  for (int16_t y=0; y < 32/scale; y++)
    tft.drawFastHLine(0, startY+y, tft.width(), 0);
  for (int16_t y=0; y < 4/scale; y++)
    tft.drawFastHLine(0, startY+32/scale+y, tft.width(), 0);
  for (int16_t y=0; y < 32/scale; y++)
    tft.drawFastHLine(0, startY+32/scale+4/scale +y, tft.width(), 0);
}

void
drawCopper(int startY, int scale, int colourShift)
{
  for (int16_t y=0; y < 32/scale; y++)
    tft.drawFastHLine(0, startY+y, tft.width(), (y*scale) << colourShift);
  for (int16_t y=0; y < 4/scale; y++)
    tft.drawFastHLine(0, startY+32/scale+y, tft.width(), 31 << colourShift);
  for (int16_t y=0; y < 32/scale; y++)
    tft.drawFastHLine(0, startY+32/scale+4/scale +y, tft.width(), (31-y*scale) << colourShift);
}

void
printIndent(int n)
{
  int i;
  
  for (i = 0; i < n; i++)
    tft.print(" ");
}

void
drawScheduleIndentedText(int n, int w, const char *text)
{
  int i, index;

  index = 0;
  i = 0;
    
  printIndent(n);
  while (text[index]) {
    int j, k;
      
    for (j = index, k = i; k < w && text[j] && text[j] != ' '; j++, k++);
    if (k == w) {
      tft.println("");
      printIndent(n);
      i = 0;
    } else {
      for (; text[index] && text[index] != ' '; index++, i++)
        tft.print(text[index]);
      if (text[index] == ' ') {
        tft.print(" ");
        i++;
        index++;
      }
    }
  }
  tft.println("");
}

void
drawScheduleTime(const char *slot, const char *talk, const char *speaker)
{
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(1);
  tft.print("\t");
  tft.printf(slot);
  tft.println("\t");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextWrap(true);
  tft.setTextSize(0);
  drawScheduleIndentedText(4, 20, talk);
}

void
drawSchedule(const char *title, struct schedule_s *schedule) {
  int index;

  index = 0;
  tft.setTextWrap(false);
  while (schedule[index].slot) {
    int i;
    
    tft.fillScreen(ST7735_BLACK);
    tft.invertDisplay(true);
    delay(100);
    //analogWrite(9,64);
    tft.invertDisplay(false);
    testtriangles();
    delay(100);  // large block of text
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(2);
    tft.println(title);
    for (i = 0; i < 2 && schedule[index].slot; i++) {    
      drawScheduleTime(schedule[index].slot, schedule[index].talk, schedule[index].speaker);
      index++;
    }
    for (i = 0; i < 100; i++) {
      if (checkButtonSelect())
        return;
      delay(20);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST7735_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testroundrects() {
  tft.fillScreen(ST7735_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 2; t+=1) {
    int x = 0;
    int y = 20;
    int w = tft.width()-2;
    int h = tft.height()-20;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
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

uint32_t bmpFilePosition;

#define USE_BMP_COMPRESSION

#ifdef USE_BMP_COMPRESSION

int inDecompression = 0;
unsigned int decompressionCount = 0;
unsigned int decompressionIndex = 0;

uint8_t
bmpRead8()
{
  uint8_t c;

  if (inDecompression == 0) {
    c = myfile[bmpFilePosition++];
    if (c)
      return c;
    inDecompression = 1;
    decompressionCount = myfile[bmpFilePosition++];
    decompressionIndex = 0;
    return 0;
  } else {
    decompressionIndex++;
    if (decompressionIndex == decompressionCount) {
      inDecompression = 0;
      return bmpRead8();
    }
    return 0;
  }
}

#else

uint8_t
bmpRead8()
{
  return myfile[bmpFilePosition++];
}

#endif

void
bmpSeek(uint32_t pos)
{
  bmpFilePosition = 0;
#ifdef USE_BMP_COMPRESSION
  inDecompression = 0;
#endif
  for (uint32_t i = 0; i < pos; i++)
    bmpRead8();
}

void
bmpFileRead(uint8_t *buf, uint32_t sz)
{
  for (uint32_t i = 0; i < sz; i++) {
    buf[i] = bmpRead8(); 
  }
}

uint16_t bmpRead16() {
  uint16_t result;
  ((uint8_t *)&result)[0] = bmpRead8(); // LSB
  ((uint8_t *)&result)[1] = bmpRead8(); // MSB
  return result;
}

uint32_t bmpRead32() {
  uint32_t result;
  ((uint8_t *)&result)[0] = bmpRead8(); // LSB
  ((uint8_t *)&result)[1] = bmpRead8();
  ((uint8_t *)&result)[2] = bmpRead8();
  ((uint8_t *)&result)[3] = bmpRead8(); // MSB
  return result;
}
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 32

void
bmpDraw(uint8_t x, uint8_t y) {
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height())) return;

  bmpFilePosition = 0;
#ifdef USE_BMP_COMPRESSION
  inDecompression = 0;
#endif
  
  // Parse BMP header
  if(bmpRead16() == 0x4D42) { // BMP signature
    bmpRead32(); // file size
    (void)bmpRead32(); // Read & ignore creator bytes
    bmpImageoffset = bmpRead32(); // Start of image data
    // Read DIB header
    bmpRead32(); // header size
    bmpWidth  = bmpRead32();
    bmpHeight = bmpRead32();
    Serial.print("bmpWidth "); Serial.println(bmpWidth);
    Serial.print("bmpHeight "); Serial.println(bmpHeight);
    if(bmpRead16() == 1) { // # planes -- must be '1'
      bmpDepth = bmpRead16(); // bits per pixel
      Serial.print("bmpDepth "); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (bmpRead32() == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFilePosition != pos) { // Need seek?
            bmpSeek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFileRead(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.Color565(r,g,b));
            //Serial.print(tft.Color565(r,g,b), HEX);
            //Serial.print(",");
          } // end pixel
          //Serial.println();
        } // end scanline
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

