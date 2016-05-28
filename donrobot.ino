#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN         7
#define MIC_PIN         0

#define TIMESPAN       25
#define WEIGHT          1
#define VOLUME_OFFSET   6
#define VOLUME_DIVIDER 27


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, LED_PIN,
                            NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB + NEO_KHZ800);

int maxMic, minMic, currMic;
int EQBar;
unsigned long timer;

void setup() {

  Serial.begin(115200);
  matrix.begin();

  resetMicReadings();
}


void loop() {

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
    matrix.fillRect(32 - EQBar, 0, EQBar, 4, matrix.Color(25, 0, 0));
    matrix.fillRect(         0, 4, EQBar, 4, matrix.Color(0, 0, 25));
    matrix.show();

  }

  delay(1);

}


void resetMicReadings() {
  maxMic = 0;
  minMic = 1024;
  timer = millis();
}
