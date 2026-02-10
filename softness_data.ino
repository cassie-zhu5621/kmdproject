#include <M5StickCPlus.h>
#include <Wire.h>

#define AS5600_ADDR 0x36
#define REG_RAW_ANGLE 0x0C
const int PRESSURE_PIN = 36; 

// --- setting the range ---
const float RANGE_P = 2000.0; 
const float RANGE_A = 60.0;  

const int ORIGIN_X = 8;   
const int ORIGIN_Y = 130; 
const int GRAPH_W = 228;  
const int GRAPH_H = 125;  

// --- variable ---
int baseP = 4095, baseA = 0;
float lastX = ORIGIN_X, lastY = ORIGIN_Y;
bool isPlotMode = false; 
int waveform[240]; 

void setup() {
  // parameter：LCDEnable=false, PowerEnable=true, SerialEnable=true
  M5.begin(false, true, true); 
  M5.Axp.begin();
  M5.Axp.ScreenBreath(0); 
  
  // starting
  M5.Lcd.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK); 
  delay(100);               
  
  Wire.begin(32, 33);
  baseP = analogRead(PRESSURE_PIN);
  baseA = getEncoderRaw();
  for(int i=0; i<240; i++) waveform[i] = 130;

  if(isPlotMode) initPlotUI();
  else drawMode1Static();
  M5.Axp.ScreenBreath(100); 
}

void loop() {
  M5.update(); 

  // close
  if (M5.BtnA.pressedFor(2000)) {
    M5.Lcd.fillScreen(RED);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(70, 50);
    M5.Lcd.print("OFF");
    delay(1000); 
    M5.Axp.PowerOff();
  }

  // --- default: return to 0 ---
  if (M5.BtnA.wasReleased()) {
    baseP = analogRead(PRESSURE_PIN);
    baseA = getEncoderRaw();
    M5.Lcd.fillScreen(BLACK);
    if(isPlotMode) initPlotUI();
    else drawMode1Static();
    lastX = ORIGIN_X; lastY = ORIGIN_Y;
    M5.Axp.ScreenBreath(100); 
    M5.Beep.tone(4000, 100);
  }

  // --- mode switch ---
  if (M5.BtnB.wasPressed()) {
    isPlotMode = !isPlotMode;
    M5.Lcd.fillScreen(BLACK);
    if(isPlotMode) initPlotUI();
    else drawMode1Static();
    lastX = ORIGIN_X; lastY = ORIGIN_Y;
    M5.Axp.ScreenBreath(100);
  }

  int curP = analogRead(PRESSURE_PIN);
  int curA = getEncoderRaw();
  float dP = (float)(baseP - curP); if (dP < 0) dP = 0;
  float dA = (float)abs(curA - baseA);
  float K = (dA > 1.0) ? (dP / dA) : 0;

  if (!isPlotMode) {
    // --- mode 1 screen ---
    M5.Lcd.setTextSize(4); 
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(15, 10);
    M5.Lcd.printf("P:%-4d", curP);
    M5.Lcd.setCursor(15, 45);
    M5.Lcd.printf("A:%-4d", curA);
    drawWaveform(K);
  } 
  else {
    // --- mode 2 screen ---
    drawXYPlot(K, dP, dA);
  }

  delay(15); 
}

// --- mode 1 real-time wave ---
void drawWaveform(float k) {
  int graphTop = 85, graphBottom = 134;
  for (int i = 0; i < 239; i++) waveform[i] = waveform[i+1];
  int yVal = map(constrain(k * 10, 0, 150), 0, 150, graphBottom, graphTop);
  waveform[239] = yVal;

  M5.Lcd.fillRect(0, 82, 240, 53, BLACK);
  M5.Lcd.drawFastHLine(0, 82, 240, DARKGREY); 
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  // adjust the place
  M5.Lcd.setCursor(140, 45); 
  M5.Lcd.printf("K:%.2f", k); 
  
  for (int i = 0; i < 239; i++) M5.Lcd.drawLine(i, waveform[i], i+1, waveform[i+1], GREEN);
}

// --- mode 2 dP-dA graph ---
void drawXYPlot(float K, float dP, float dA) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(YELLOW, BLACK);
  M5.Lcd.setCursor(155, 5); 
  M5.Lcd.printf("K:%.2f", K); 

  int targetX = ORIGIN_X + (int)((dA / RANGE_A) * GRAPH_W);
  int targetY = ORIGIN_Y - (int)((dP / RANGE_P) * GRAPH_H);

  targetX = constrain(targetX, ORIGIN_X, 239);
  targetY = constrain(targetY, 0, ORIGIN_Y);

  if (dA > 0.5 || dP > 2.0) {
    uint16_t color = (K > 5.0) ? RED : GREEN;
    M5.Lcd.drawLine((int)lastX, (int)lastY, targetX, targetY, color);
    lastX = (float)targetX;
    lastY = (float)targetY;
  }
}

int getEncoderRaw() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(REG_RAW_ANGLE);
  Wire.endTransmission(false);
  Wire.requestFrom(AS5600_ADDR, 2);
  if (Wire.available() >= 2) return ((Wire.read() << 8 | Wire.read()) & 0x0FFF);
  return 0;
}

void initPlotUI() {
  M5.Lcd.drawLine(ORIGIN_X, ORIGIN_Y, 239, ORIGIN_Y, WHITE); 
  M5.Lcd.drawLine(ORIGIN_X, ORIGIN_Y, ORIGIN_X, 0, WHITE);   
}

void drawMode1Static() {
  M5.Lcd.drawFastHLine(0, 82, 240, DARKGREY);
}