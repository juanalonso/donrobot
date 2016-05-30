#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN         7
#define MIC_PIN         0
#define BUT_PIN         2

#define TIMESPAN       25
#define WEIGHT          1
#define VOLUME_OFFSET   6
#define VOLUME_DIVIDER 27

#define MAT_W          32
#define MAT_H           8

#define COL_MED        255
#define COL_MIN        255

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MAT_W, MAT_H, LED_PIN,
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

int maxMic, minMic, currMic;
int EQBar;
unsigned long timer;
int currentPreset = 1;
boolean button_was_pressed = false;

int p_kitt_xpos = MAT_W / 2;
int p_kitt_delta = 1;
boolean p_eyes_open = true;

void setup() {

  Serial.begin(115200);
  matrix.begin();
  randomSeed(analogRead(5));
  pinMode(BUT_PIN, INPUT);
  digitalWrite(BUT_PIN, HIGH);

  resetMicReadings();
}


void loop() {

  int button_pressed = !digitalRead(BUT_PIN);
  if (!button_pressed && button_was_pressed) {
    currentPreset++;
    if (currentPreset > 4) {
      currentPreset = 1;
    } else if (currentPreset == 2) {
      p_eyes_open = true;
    } else if (currentPreset == 4) {
      patch_smiley();
    }
    delay(25);
  }
  button_was_pressed = button_pressed;

  switch (currentPreset) {
    case 1:
      patch_volume();
      break;
    case 2:
      patch_eyes();
      break;
    case 3:
      patch_kitt();
      break;
    case 4:
      delay(25);
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
    matrix.fillRect(MAT_W - EQBar,         0, EQBar, MAT_H / 2, matrix.Color(COL_MIN, 0, 0));
    matrix.fillRect(            0, MAT_H / 2, EQBar, MAT_H / 2, matrix.Color(0, 0, COL_MIN));
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

  if (p_eyes_open) {

    if (millis() - timer >= TIMESPAN * 6) {

      int pupil_x = random(0, 5);
      int pupil_y = random(0, 5);

      matrix.fillScreen(0);
      drawOpenEye(4, pupil_x, pupil_y);
      drawOpenEye(4 + MAT_W / 2, pupil_x, pupil_y);
      matrix.show();

      timer = millis();
      p_eyes_open = !p_eyes_open;

    }

  } else {

    if (millis() - timer >= TIMESPAN * 100) {

      matrix.fillScreen(0);
      drawClosedEye(4);
      drawClosedEye(4 + MAT_W / 2);
      matrix.show();

      timer = millis();
      p_eyes_open = !p_eyes_open;

    }

  }

}


void drawOpenEye(int eye_offset, int p_x, int p_y) {

  p_x++;
  p_y++;

  matrix.drawFastHLine(eye_offset + 1,         0, 8 - 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
  matrix.drawFastHLine(eye_offset + 1,         7, 8 - 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
  matrix.drawFastVLine(eye_offset,         0 + 1, 8 - 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
  matrix.drawFastVLine(eye_offset + 8 - 1, 0 + 1, 8 - 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
  matrix.drawRect(eye_offset + p_x, p_y, 2, 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
}


void drawClosedEye(int eye_offset) {
  matrix.drawFastHLine(eye_offset, 3, 8, matrix.Color(COL_MED, COL_MIN, COL_MIN));
  matrix.drawFastHLine(eye_offset, 4, 8, matrix.Color(COL_MED, COL_MIN, COL_MIN));
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

  delay(25);

}


void patch_smiley() {

  matrix.fillScreen(0);
  drawSmiley(4);
  drawSmiley(4 + MAT_W / 2);
  matrix.show();

}


void drawSmiley(int eye_offset) {

  matrix.drawFastHLine(eye_offset + 1,         0, 8 - 2, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawFastHLine(eye_offset + 1,         7, 8 - 2, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawFastVLine(eye_offset,         0 + 1, 8 - 2, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawFastVLine(eye_offset + 8 - 1, 0 + 1, 8 - 2, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 2, 2, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 2, 4, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 3, 5, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 4, 5, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 5, 4, matrix.Color(COL_MED, COL_MED, 0));
  matrix.drawPixel    (eye_offset + 5, 2, matrix.Color(COL_MED, COL_MED, 0));
  //matrix.drawRect(eye_offset + p_x, p_y, 2, 2, matrix.Color(COL_MED, COL_MIN, COL_MIN));
}

