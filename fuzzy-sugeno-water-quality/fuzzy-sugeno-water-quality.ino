#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WaterQualityCalibrator.h>

// Pin sensor
#define PH_PIN    A2
#define TURB_PIN  A1
#define TDS_PIN   A0

WaterQualityCalibrator waterSensor(PH_PIN, TURB_PIN, TDS_PIN);
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long lastPrintTime = 0;
const unsigned long interval = 2000;
String serialBuffer = "";

// Fungsi keanggotaan trapezoidal
float mfAsam(float ph) {
  if (ph <= 5.5) return 1;
  else if (ph >= 6.5) return 0;
  else return (6.5 - ph) / 1.0;
}

float mfNetral(float ph) {
  if (ph <= 6.5 || ph >= 8.5) return 0;
  else if (ph < 7.5) return (ph - 6.5) / 1.0;
  else return (8.5 - ph) / 1.0;
}

float mfBasa(float ph) {
  if (ph <= 8.5) return 0;
  else if (ph >= 9.5) return 1;
  else return (ph - 8.5) / 1.0;
}

float mfBaik(float tds) {
  if (tds <= 500) return 1;
  else if (tds >= 700) return 0;
  else return (700 - tds) / 200.0;
}

float mfCukup(float tds) {
  if (tds <= 500 || tds >= 1000) return 0;
  else if (tds < 750) return (tds - 500) / 250.0;
  else return (1000 - tds) / 250.0;
}

float mfTidakBaik(float tds) {
  if (tds <= 1000) return 0;
  else if (tds >= 1200) return 1;
  else return (tds - 1000) / 200.0;
}

float mfJernih(float turb) {
  if (turb <= 5) return 1;
  else if (turb >= 10) return 0;
  else return (10 - turb) / 5.0;
}

float mfCukupTurb(float turb) {
  if (turb <= 5 || turb >= 25) return 0;
  else if (turb < 15) return (turb - 5) / 10.0;
  else return (25 - turb) / 10.0;
}

float mfKeruh(float turb) {
  if (turb <= 25) return 0;
  else if (turb >= 30) return 1;
  else return (turb - 25) / 5.0;
}

float fuzzySugenoOrder1(float ph, float tds, float turb) {
  struct Rule {
    float (*mfPH)(float);
    float (*mfTDS)(float);
    float (*mfTurb)(float);
    float a, b, c, d;
  };

  Rule rules[] = {
    { mfAsam, mfTidakBaik, mfKeruh,      0, 0, 0, 0.0 },
    { mfAsam, mfTidakBaik, mfCukupTurb, 0, 0, 0, 0.2 },
    { mfAsam, mfTidakBaik, mfJernih,    0.1, 0.1, 0.1, 0.3 },
    { mfAsam, mfCukup,     mfKeruh,     0, 0, 0, 0.1 },
    { mfAsam, mfCukup,     mfCukupTurb, 0.1, 0.1, 0.1, 0.3 },
    { mfAsam, mfCukup,     mfJernih,    0.2, 0.1, 0.1, 0.4 },
    { mfAsam, mfBaik,      mfKeruh,     0.2, 0.1, 0.1, 0.4 },
    { mfAsam, mfBaik,      mfCukupTurb, 0.2, 0.2, 0.1, 0.5 },
    { mfAsam, mfBaik,      mfJernih,    0.2, 0.2, 0.2, 0.5 },
    { mfNetral, mfTidakBaik, mfKeruh,   0, 0, 0, 0.1 },
    { mfNetral, mfTidakBaik, mfCukupTurb, 0.1, 0.1, 0.1, 0.2 },
    { mfNetral, mfTidakBaik, mfJernih,  0.1, 0.1, 0.1, 0.4 },
    { mfNetral, mfCukup, mfKeruh,       0.1, 0.1, 0.1, 0.4 },
    { mfNetral, mfCukup, mfCukupTurb,   0.2, 0.2, 0.2, 0.5 },
    { mfNetral, mfCukup, mfJernih,      0.2, 0.2, 0.2, 0.7 },
    { mfNetral, mfBaik, mfKeruh,        0.2, 0.2, 0.1, 0.5 },
    { mfNetral, mfBaik, mfCukupTurb,    0.2, 0.2, 0.2, 0.7 },
    { mfNetral, mfBaik, mfJernih,       0.2, 0.2, 0.2, 0.8 },
    { mfBasa, mfTidakBaik, mfKeruh,     0, 0, 0, 0.1 },
    { mfBasa, mfTidakBaik, mfCukupTurb, 0.1, 0.1, 0.1, 0.2 },
    { mfBasa, mfTidakBaik, mfJernih,    0.1, 0.1, 0.1, 0.3 },
    { mfBasa, mfCukup, mfKeruh,         0.1, 0.1, 0.1, 0.4 },
    { mfBasa, mfCukup, mfCukupTurb,     0.2, 0.2, 0.2, 0.5 },
    { mfBasa, mfCukup, mfJernih,        0.2, 0.2, 0.2, 0.7 },
    { mfBasa, mfBaik, mfKeruh,          0.2, 0.2, 0.2, 0.6 },
    { mfBasa, mfBaik, mfCukupTurb,      0.2, 0.2, 0.2, 0.8 },
    { mfBasa, mfBaik, mfJernih,         0.2, 0.2, 0.2, 1.0 }
  };

  float sumWeightedOutput = 0;
  float sumWeights = 0;

  for (auto& r : rules) {
    float w = r.mfPH(ph) * r.mfTDS(tds) * r.mfTurb(turb);
    float z = r.a * ph + r.b * tds + r.c * turb + r.d;
    sumWeightedOutput += w * z;
    sumWeights += w;
  }

  if (sumWeights == 0) return 0.5;
  return sumWeightedOutput / sumWeights;
}

String interpretOutput(float score) {
  if (score < 0.4) return "BURUK";
  else if (score < 0.7) return "KURANG BAIK";
  else return "BAIK";
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  waterSensor.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Quality Test");
  delay(1500);
  lcd.clear();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (serialBuffer.length() > 0) {
        int result = waterSensor.handleCommand(serialBuffer);
        if (result == 0) Serial.println(">> Unknown or invalid command");
        serialBuffer = "";
      }
    } else {
      serialBuffer += c;
    }
  }

  unsigned long currentTime = millis();
  if (currentTime - lastPrintTime >= interval) {
    lastPrintTime = currentTime;

    float temp = 25.0;
    float ph = waterSensor.readPH();
    float turb = waterSensor.readTurbidity();
    float tds = waterSensor.readTDS(temp);

    float fuzzyScore = fuzzySugenoOrder1(ph, tds, turb);
    String kualitas = interpretOutput(fuzzyScore);

    Serial.print("pH: "); Serial.print(ph, 2);
    Serial.print(" | Turbidity: "); Serial.print(turb, 1);
    Serial.print(" | TDS: "); Serial.print(tds, 1);
    Serial.print(" => "); Serial.println(kualitas);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("pH: "); lcd.print(ph, 2);
    lcd.setCursor(0, 1);
    lcd.print("Turb: "); lcd.print(turb, 1);
    lcd.setCursor(0, 2);
    lcd.print("TDS: "); lcd.print(tds, 1);
    lcd.setCursor(0, 3);
    lcd.print("Qual: "); lcd.print(kualitas);
  }
}
