#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN         7
#define MIC_PIN         0

#define TIMESPAN       25
#define WEIGHT          1
#define VOLUME_OFFSET   6
#define VOLUME_DIVIDER 27

#define MAT_W          32
#define MAT_H           8

enum presets {
  P_VOLUME,
  P_EYES,
  P_KITT
};

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MAT_W, MAT_H, LED_PIN,
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

int maxMic, minMic, currMic;
int EQBar;
unsigned long timer;
presets currentPreset = P_KITT;

int p_kitt_xpos = MAT_W / 2;
int p_kitt_delta = 1;

void setup() {

  Serial.begin(115200);
  matrix.begin();

  resetMicReadings();
}


void loop() {

  switch (currentPreset) {
    case P_VOLUME:
      patch_volume();
      break;
    case P_EYES:
      patch_eyes();
      break;
    case P_KITT:
      patch_kitt();
      break;
  }

}


void patch_volume() {
  currMic = analogRead(MIC_PIN);
  //Serial.println(currMic, DEC);

  if (currMic < minMic) {
    minMic = currMic;
  }

  if (currMic > maxMic) {
    maxMic = currMic;
  }

  if (millis() - timer >= TIMESPAN) {

    EQBar = (maxMic - minMic) * WEIGHT + EQBar * (1.0 - WEIGHT);
    EQBar = EQBar / VOLUME_DIVIDER - VOLUME_OFFSET;
    //Serial.println(EQBar);

    resetMicReadings();

    matrix.fillScreen(0);
    matrix.fillRect(MAT_W - EQBar,         0, EQBar, MAT_H / 2, matrix.Color(25, 0, 0));
    matrix.fillRect(            0, MAT_H / 2, EQBar, MAT_H / 2, matrix.Color(0, 0, 25));
    matrix.show();

  }

  delay(1);
}


void resetMicReadings() {
  maxMic = 0;
  minMic = 1024;
  timer = millis();
}


void patch_eyes() {

  matrix.fillScreen(0);
  matrix.fillRect(MAT_W - EQBar, 0, EQBar, MAT_H / 2, matrix.Color(25, 0, 0));
  matrix.fillRect(         0, MAT_H / 2, EQBar, MAT_H / 2, matrix.Color(0, 0, 25));
  matrix.show();

  delay(10);
}


void patch_kitt() {

  p_kitt_xpos += p_kitt_delta;

  if (p_kitt_xpos >= MAT_W - 3) {
    p_kitt_delta = -1;
    p_kitt_xpos = MAT_W - 3;
  } else if (p_kitt_xpos < 3) {
    p_kitt_delta = 1;
    p_kitt_xpos = 2;
  }


  matrix.fillScreen(0);
  matrix.drawFastVLine(p_kitt_xpos - 2, 0, MAT_H, matrix.Color(64, 0, 0));
  matrix.drawFastVLine(p_kitt_xpos - 1, 0, MAT_H, matrix.Color(128, 0, 0));
  matrix.drawFastVLine(p_kitt_xpos, 0, MAT_H, matrix.Color(255, 0, 0));
  matrix.drawFastVLine(p_kitt_xpos + 1, 0, MAT_H, matrix.Color(128, 0, 0));
  matrix.drawFastVLine(p_kitt_xpos + 2, 0, MAT_H, matrix.Color(64, 0, 0));
  matrix.show();
  
  delay(10);

}
