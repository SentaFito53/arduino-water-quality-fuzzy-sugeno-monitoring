#ifndef WATER_QUALITY_CALIBRATOR_H
#define WATER_QUALITY_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>

#define TDS_VREF 5.0
#define TDS_ADC_RES 1024.0
#define TDS_FACTOR 0.5
#define TDS_EEPROM_ADDR 120

class WaterQualityCalibrator {
public:
    WaterQualityCalibrator(uint8_t ph, uint8_t turb, uint8_t tds);
    void begin();

    float readVoltage(uint8_t pin);
    float readPH();
    float readTurbidity();
    float readTDS(float temperature);

    int handleCommand(const String& input);
    void updateTDSCalibration();

private:
    static const int MAX_SAMPLES = 6;

    uint8_t phPin, turbPin, tdsPin;
    float temperature;
    float tdsVoltage;

    // pH calibration
    float phValues[2];
    float phVoltages[2];
    int phSampleCount;
    float phSlope, phIntercept;

    // Turbidity calibration
    float turbValues[MAX_SAMPLES];
    float turbVoltages[MAX_SAMPLES];
    int turbSampleCount;
    float turbSlope, turbIntercept;

    // TDS calibration
    float tdsKValue;
    bool tdsCalibReady;

    // Flags
    bool inPHCalib, inTurbCalib, inTDSCalib;

    // Calibration utilities
    void calcPHRegression();
    void calcTurbRegression();
    byte parseTDSCommand(const String& cmd, float& calVal);

    void loadPHCalibration();
    void savePHCalibration();
    void loadTurbCalibration();
    void saveTurbCalibration();
    void loadTDSCalibration();
    void saveTDSCalibration(float k);
};

#endif
