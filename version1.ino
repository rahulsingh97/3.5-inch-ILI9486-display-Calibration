// first adjust the center dot alighment and then we can adjust the edges 

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

// Current pin setup
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// Current calibration - we'll adjust these for centering the crossheir 
#define TS_MINX 220 
#define TS_MINY 140
#define TS_MAXX 920 
#define TS_MAXY 940


// For better touch detection
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define WHITE 0xFFFF
#define GRAY 0x8410

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Variables to store touch data
int last_x = 0;
int last_y = 0;

void setup() {
  Serial.begin(9600);
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1); // Landscape
  
//serial print tft.width() and tft.height() to get the screen size 
  Serial.print("Screen size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  int diplay_width = tft.width()-30; // for calibration if xy is over shoting
  int diplay_height = tft.height()-30; // for calibration if xy is over shoting

  int diplay_width_1 = +30; // for calibration if xy is over shoting
  int diplay_height_1 = +30; // for calibration if xy is over shoting
  // Draw initial screen
  initScreen();
}

void initScreen() {
  tft.fillScreen(BLACK);
  
  // Draw touch target points
  drawCrossHair(50, 50);
  drawCrossHair(tft.width()-50, 50);
  drawCrossHair(tft.width()-50, tft.height()-50);
  drawCrossHair(50, tft.height()-50);
  drawCrossHair(tft.width()/2, tft.height()/2);
  
  // Draw instructions
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(10, 10);
  tft.print("Touch the screen");
  
  // Draw data box
  tft.drawRect(5, tft.height()-60, tft.width()-10, 55, WHITE);
}

void drawCrossHair(int x, int y) {
  tft.drawLine(x-10, y, x+10, y, GREEN);
  tft.drawLine(x, y-10, x, y+10, GREEN);
  tft.drawCircle(x, y, 5, GREEN);
}

void displayTouchData(TSPoint p, int mapped_x, int mapped_y) {
  // Clear previous data
  tft.fillRect(6, tft.height()-59, tft.width()-12, 53, BLACK);
  
  // Display raw and mapped values
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(10, tft.height()-50);
  tft.print("Raw X: ");
  tft.print(p.x);
  tft.print(" Y: ");
  tft.print(p.y);
  tft.print(" Pressure: ");
  tft.println(p.z);
  
  tft.setCursor(10, tft.height()-35);
  tft.print("Mapped X: ");
  tft.print(mapped_x);
  tft.print(" Y: ");
  tft.println(mapped_y);
  
  // Display suggested calibration values
  tft.setCursor(10, tft.height()-20);
  tft.print("Min X: ");
  tft.print(min(last_x, p.x));
  tft.print(" Max X: ");
  tft.print(max(last_x, p.x));
  tft.print(" Min Y: ");
  tft.print(min(last_y, p.y));
  tft.print(" Max Y: ");
  tft.print(max(last_y, p.y));
  
  // Update last values
  last_x = p.x;
  last_y = p.y;
}

void drawTouchPoint(int x, int y) {
  static int last_mapped_x = 0;
  static int last_mapped_y = 0;
  
  // Draw line from last point to new point
  if (last_mapped_x != 0 && last_mapped_y != 0) {
    tft.drawLine(last_mapped_x, last_mapped_y, x, y, RED);
  }
  
  // Draw new point
  tft.fillCircle(x, y, 3, BLUE);
  
  last_mapped_x = x;
  last_mapped_y = y;
}


void loop() {  
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  
  // Restore pins that are shared between touch and LCD
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // Map the raw touch coordinates to screen coordinates
    int mapped_x = map(p.y, TS_MINX, TS_MAXX, diplay_width, diplay_width_1);  // Inverted X 
    int mapped_y = map(p.x, TS_MINY, TS_MAXY, diplay_height, diplay_height_1); // Inverted Y
    // if x and y are inverted try changing p.y to p.x and p.x to p.y
    // if issue still exist swap diplay_width with diplay_width_1 and swap diplay_height with diplay_height_1 
    
    // Draw touch point and display data
    drawTouchPoint(mapped_x, mapped_y);
    displayTouchData(p, mapped_x, mapped_y);
    
    // Also send to serial for debugging
    Serial.print("Raw X = "); Serial.print(p.x);
    Serial.print("\tRaw Y = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.print(p.z);
    Serial.print("\tMapped X = "); Serial.print(mapped_x);
    Serial.print("\tMapped Y = "); Serial.println(mapped_y);
  }
}
