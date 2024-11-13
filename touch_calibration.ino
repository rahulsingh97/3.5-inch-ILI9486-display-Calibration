#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// Current calibration values (can be modified via Serial)
int TS_LEFT = 200;
int TS_RT = 1000;
int TS_TOP = 1000;
int TS_BOT = 200;
int TOUCH_THRESHOLD = 300;

// Touch screen pins
const int XP = 8;
const int XM = A2;
const int YP = A3;
const int YM = 9;

// Colors
#define GREEN 0x07E0
#define RED 0xF800
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0x001F

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); // 300 ohm resistor

void setup() {
  Serial.begin(9600);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  tft.reset();
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(0);  // Portrait orientation
  
  Serial.println(F("Touch Screen Calibration Tool v2.0"));
  printHelpCommands();
  Serial.println(F("\nFor 3.5 TFT LCD Shield with ILI9488 controller"));
  Serial.println(F("Built with Love by Rahul: https://github.com/rahulsingh97"));

  showGreetingScreen();
}

void drawCrosshair(int x, int y) {
  tft.drawLine(x - 10, y, x + 10, y, GREEN);
  tft.drawLine(x, y - 10, x, y + 10, GREEN);
  // Add circle around crosshair for better visibility
  tft.drawCircle(x, y, 15, GREEN);
}

void showGreetingScreen() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  
  // Calculate center position for title
  int centerX = tft.width() / 2;
  tft.setCursor(10, 10);
  tft.println(F("Touch Screen"));
  tft.setCursor(10, 30);
  tft.println(F("Calibration Tool v2.0"));
  
  tft.setTextSize(1);
  tft.setCursor(10, 60);
  tft.println(F("Instructions:"));
  tft.setTextSize(1);
  tft.setCursor(10, 80);
  tft.println(F("1. Touch crosshairs precisely"));
  tft.setCursor(10, 100);
  tft.println(F("2. Adjust values via Serial"));
  tft.setCursor(10, 120);
  tft.println(F("3. Use 'print' for final values"));
  tft.setCursor(10, 140);
  tft.println(F("4. Use 'help' for commands"));
  tft.setCursor(10, 180);
  tft.setTextSize(1);
  tft.println(F("Desription:"));
  tft.setTextSize(1);
  tft.setCursor(10, 200);
  tft.println(F("The Touch Screen Calibration Tool is an essential utility designed to ensure precise touch input on ILI9486 displays with Arduino. By displaying five strategic crosshairs on the screen, this tool helps map the raw touch sensor values to actual screen coordinates. When you touch the screen, it shows a red dot at the detected position and sends the raw touch values to the serial monitor, allowing you to visually and numerically assess the accuracy of your touch detection. Through simple serial commands, you can adjust the calibration values (TS_LEFT, TS_RIGHT, TS_TOP, TS_BOTTOM) in real-time and instantly see the results, making it easy to achieve perfect touch accuracy for your interactive projects."));
  tft.setCursor(centerX, tft.height() - 70);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.println(F("Made by Rahul"));

  tft.setTextSize(1);
  tft.setTextColor(RED);
  tft.setCursor(10, tft.height() - 30);
  tft.println(F("Wait 10 seconds to begin..."));
  tft.setTextColor(BLUE);
  // Sleep for 10 seconds just to show greeting screen
  delay(15000);
  tft.println(F("Touch screen to begin calibration"));
  // Wait for touch to continue
  waitForTouch();
  drawCalibrationScreen();
}

void waitForTouch() {
  while (true) {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
    
    // Restore pins - important!
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > TOUCH_THRESHOLD) {
      delay(100);  // Debounce
      return;
    }
  }
}

void drawCalibrationScreen() {
  tft.fillScreen(BLACK);
  
  // Get screen dimensions
  int width = tft.width();
  int height = tft.height();
  
  // Draw crosshairs (25 pixels from edges for corners)
  drawCrosshair(25, 25);  // Top-left
  drawCrosshair(width - 25, 25);  // Top-right
  drawCrosshair(25, height - 25);  // Bottom-left
  drawCrosshair(width - 25, height - 25);  // Bottom-right
  drawCrosshair(width/2, height/2);  // Center
}

void printCalibrationValues() {
  Serial.println(F("\n--- Calibration Values ---"));
  
  Serial.println(F("\nPortrait Mode:"));
  Serial.println(F("const int XP=8, XM=A2, YP=A3, YM=9;"));
  Serial.print(F("const int TS_LEFT=")); Serial.print(TS_LEFT);
  Serial.print(F(", TS_RT=")); Serial.print(TS_RT);
  Serial.print(F(", TS_TOP=")); Serial.print(TS_TOP);
  Serial.print(F(", TS_BOT=")); Serial.println(TS_BOT);
  Serial.print(F("const int TOUCH_THRESHOLD = ")); Serial.println(TOUCH_THRESHOLD);
  
  Serial.println(F("\nLandscape Mode:"));
  Serial.println(F("const int XP=8, XM=A2, YP=A3, YM=9;"));
  Serial.print(F("const int TS_LEFT=")); Serial.print(TS_TOP);
  Serial.print(F(", TS_RT=")); Serial.print(TS_BOT);
  Serial.print(F(", TS_TOP=")); Serial.print(TS_LEFT);
  Serial.print(F(", TS_BOT=")); Serial.println(TS_RT);
  Serial.print(F("const int TOUCH_THRESHOLD = ")); Serial.println(TOUCH_THRESHOLD);
  
  Serial.println(F("-------------------"));
}

void printHelpCommands() {
  Serial.println(F("\nAvailable Commands:"));
  Serial.println(F("clear     - Clear screen"));
  Serial.println(F("left=XXX  - Set TS_LEFT"));
  Serial.println(F("right=XXX - Set TS_RT"));
  Serial.println(F("top=XXX   - Set TS_TOP"));
  Serial.println(F("bot=XXX   - Set TS_BOT"));
  Serial.println(F("touch=XXX - Set TOUCH_THRESHOLD"));
  Serial.println(F("print     - Show calibration values"));
  Serial.println(F("help      - Show this help"));
}

void processSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == F("clear")) {
      Serial.println(F("Clearing screen..."));
      drawCalibrationScreen();
    }
    else if (command.startsWith(F("left="))) {
      TS_LEFT = command.substring(5).toInt();
      Serial.print(F("TS_LEFT set to: ")); Serial.println(TS_LEFT);
    }
    else if (command.startsWith(F("right="))) {
      TS_RT = command.substring(6).toInt();
      Serial.print(F("TS_RT set to: ")); Serial.println(TS_RT);
    }
    else if (command.startsWith(F("top="))) {
      TS_TOP = command.substring(4).toInt();
      Serial.print(F("TS_TOP set to: ")); Serial.println(TS_TOP);
    }
    else if (command.startsWith(F("bot="))) {
      TS_BOT = command.substring(4).toInt();
      Serial.print(F("TS_BOT set to: ")); Serial.println(TS_BOT);
    }
    else if (command.startsWith(F("touch="))) {
      TOUCH_THRESHOLD = command.substring(6).toInt();
      ts.pressureThreshhold = TOUCH_THRESHOLD;
      Serial.print(F("Touch threshold set to: ")); Serial.println(TOUCH_THRESHOLD);
    }
    else if (command == F("print")) {
      printCalibrationValues();
    }
    else if (command == F("help")) {
      printHelpCommands();
    }
    else {
      Serial.println(F("Invalid command. Showing 'help' insted."));
      printHelpCommands();
    }
  }
}

void loop() {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // Important: Restore pins after each reading
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > TOUCH_THRESHOLD) {
    int x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
    int y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    
    tft.fillCircle(x, y, 3, RED);
    
    Serial.println(F("\n--- Touch Detected ---"));
    Serial.print(F("Raw (x,y): ")); Serial.print(p.x); Serial.print(F(",")); Serial.println(p.y);
    Serial.print(F("Map (x,y): ")); Serial.print(x); Serial.print(F(",")); Serial.println(y);
    Serial.print(F("Pressure:  ")); Serial.println(p.z);
    
    delay(50); // Debounce
  }
  
  processSerialCommands();
}
