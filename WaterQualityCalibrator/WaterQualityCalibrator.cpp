#include "WaterQualityCalibrator.h"

WaterQualityCalibrator::WaterQualityCalibrator(uint8_t ph, uint8_t turb, uint8_t tds) {
    phPin = ph;
    turbPin = turb;
    tdsPin = tds;

    phSampleCount = 0;
    turbSampleCount = 0;

    phSlope = -5.0;
    phIntercept = 21.0;

    turbSlope = -100.0;
    turbIntercept = 300.0;

    inPHCalib = false;
    inTurbCalib = false;
    inTDSCalib = false;

    tdsCalibReady = false;
    tdsKValue = 1.0;
}

void WaterQualityCalibrator::begin() {
    loadPHCalibration();
    loadTurbCalibration();
    loadTDSCalibration();
}

float WaterQualityCalibrator::readVoltage(uint8_t pin) {
    return analogRead(pin) * TDS_VREF / TDS_ADC_RES;
}

float WaterQualityCalibrator::readPH() {
    float v = readVoltage(phPin);
    return phSlope * v + phIntercept;
}

float WaterQualityCalibrator::readTurbidity() {
    float v = readVoltage(turbPin);
    float ntu = turbSlope * v + turbIntercept;
    return max(ntu, 0.0);
}

float WaterQualityCalibrator::readTDS(float temp) {
    temperature = temp;
    float voltage = readVoltage(tdsPin);
    float ec = (133.42 * pow(voltage, 3) - 255.86 * pow(voltage, 2) + 857.39 * voltage) * tdsKValue;
    float ec25 = ec / (1.0 + 0.02 * (temp - 25.0));
    float tds = ec25 * TDS_FACTOR;
    return tds;
}

// ========== pH Calibration ==========
void WaterQualityCalibrator::loadPHCalibration() {
    EEPROM.get(100, phSlope);
    EEPROM.get(104, phIntercept);
    if (isnan(phSlope) || isnan(phIntercept)) {
        phSlope = -5.0;
        phIntercept = 21.0;
    }
}

void WaterQualityCalibrator::savePHCalibration() {
    EEPROM.put(100, phSlope);
    EEPROM.put(104, phIntercept);
}

void WaterQualityCalibrator::calcPHRegression() {
    float x1 = phVoltages[0], y1 = phValues[0];
    float x2 = phVoltages[1], y2 = phValues[1];
    phSlope = (y2 - y1) / (x2 - x1);
    phIntercept = y1 - phSlope * x1;
    savePHCalibration();
}

// ========== Turbidity Calibration ==========
void WaterQualityCalibrator::loadTurbCalibration() {
    EEPROM.get(110, turbSlope);
    EEPROM.get(114, turbIntercept);
    if (isnan(turbSlope) || isnan(turbIntercept)) {
        turbSlope = -100.0;
        turbIntercept = 300.0;
    }
}

void WaterQualityCalibrator::saveTurbCalibration() {
    EEPROM.put(110, turbSlope);
    EEPROM.put(114, turbIntercept);
}

void WaterQualityCalibrator::calcTurbRegression() {
    float sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
    for (int i = 0; i < turbSampleCount; i++) {
        sumX += turbVoltages[i];
        sumY += turbValues[i];
        sumXY += turbVoltages[i] * turbValues[i];
        sumXX += turbVoltages[i] * turbVoltages[i];
    }
    float n = turbSampleCount;
    turbSlope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    turbIntercept = (sumY - turbSlope * sumX) / n;
    saveTurbCalibration();

    Serial.print(">> Regression: NTU = ");
    Serial.print(turbSlope, 4); Serial.print(" * V + ");
    Serial.println(turbIntercept, 4);
}

// ========== TDS Calibration ==========
void WaterQualityCalibrator::loadTDSCalibration() {
    EEPROM.get(TDS_EEPROM_ADDR, tdsKValue);
    if (isnan(tdsKValue) || tdsKValue <= 0) tdsKValue = 1.0;
}

void WaterQualityCalibrator::saveTDSCalibration(float k) {
    tdsKValue = k;
    EEPROM.put(TDS_EEPROM_ADDR, k);
}

byte WaterQualityCalibrator::parseTDSCommand(const String& cmd, float& calVal) {
    if (cmd == "ENTERTDS") return 1;
    if (cmd.startsWith("CALTDS:")) {
        calVal = cmd.substring(7).toFloat();
        return 2;
    }
    if (cmd == "EXITTDS") return 3;
    return 0;
}

// ========== Command Handler ==========
int WaterQualityCalibrator::handleCommand(const String& input) {
    // === pH ===
    if (input == "ENTERPH") {
        inPHCalib = true;
        phSampleCount = 0;
        Serial.println(">> Enter pH calibration mode");
        return 1;
    }
    if (input.startsWith("PH:") && inPHCalib) {
        if (phSampleCount < 2) {
            phValues[phSampleCount] = input.substring(3).toFloat();
            phVoltages[phSampleCount] = readVoltage(phPin);
            Serial.print(">> PH Sample "); Serial.print(phSampleCount + 1); Serial.print(": ");
            Serial.print("V="); Serial.print(phVoltages[phSampleCount], 4);
            Serial.print(", pH="); Serial.println(phValues[phSampleCount]);
            phSampleCount++;
        }
        if (phSampleCount >= 2) calcPHRegression();
        return 2;
    }
    if (input == "EXITPH" && inPHCalib) {
        inPHCalib = false;
        Serial.println(">> Exit pH calibration");
        return 3;
    }

    // === Turbidity ===
    if (input == "ENTERTURB") {
        inTurbCalib = true;
        turbSampleCount = 0;
        Serial.println(">> Enter turbidity calibration mode");
        return 1;
    }
    if (input.startsWith("TURB:") && inTurbCalib) {
        if (turbSampleCount < MAX_SAMPLES) {
            turbValues[turbSampleCount] = input.substring(5).toFloat();
            turbVoltages[turbSampleCount] = readVoltage(turbPin);
            Serial.print(">> Turbidity Sample "); Serial.print(turbSampleCount + 1); Serial.print(": ");
            Serial.print("V="); Serial.print(turbVoltages[turbSampleCount], 4);
            Serial.print(", NTU="); Serial.println(turbValues[turbSampleCount]);
            turbSampleCount++;
        }
        if (turbSampleCount >= MAX_SAMPLES) calcTurbRegression();
        return 2;
    }
    if (input == "EXITTURB" && inTurbCalib) {
        inTurbCalib = false;
        Serial.println(">> Exit turbidity calibration");
        return 3;
    }

    // === TDS ===
    float calVal;
    byte mode = parseTDSCommand(input, calVal);
    switch (mode) {
        case 1:
            inTDSCalib = true;
            Serial.println(">> Enter TDS calibration mode");
            return 4;
        case 2:
            if (inTDSCalib) {
                float rawEC = calVal / TDS_FACTOR;
                rawEC *= (1.0 + 0.02 * (temperature - 25.0));
                tdsVoltage = readVoltage(tdsPin);
                float tmpK = rawEC / (133.42 * pow(tdsVoltage, 3) - 255.86 * pow(tdsVoltage, 2) + 857.39 * tdsVoltage);
                if (tmpK > 0.25 && tmpK < 4.0) {
                    tdsKValue = tmpK;
                    tdsCalibReady = true;
                    Serial.print(">> TDS K confirmed: "); Serial.println(tmpK, 4);
                } else {
                    Serial.println(">> Invalid TDS K value, try again");
                }
                return 5;
            }
            break;
        case 3:
            if (inTDSCalib) {
                if (tdsCalibReady) {
                    saveTDSCalibration(tdsKValue);
                    Serial.println(">> TDS calibration saved and exited");
                } else {
                    Serial.println(">> TDS calibration not saved, K invalid");
                }
                inTDSCalib = false;
                tdsCalibReady = false;
                return 6;
            }
            break;
    }

    return 0;
}

void WaterQualityCalibrator::updateTDSCalibration() {
    // Optional, if needed in loop()
}
