#ifndef WATER_QUALITY_CALIBRATOR_H
#define WATER_QUALITY_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>

#define TDS_VREF 5.0
#define TDS_ADC_RES 1024.0
#define TDS_EEPROM_ADDR 120
#define TDS_FACTOR 0.5

class WaterQualityCalibrator {
public:
    WaterQualityCalibrator(uint8_t phPin, uint8_t turbPin, uint8_t tdsPin);
    void begin();

    float readPH();
    float readTurbidity();
    float readTDS(float temperature);

    int handleCommand(const String& input);
    void updateTDSCalibration(); // call this in loop()

private:
    // Pin assignments
    uint8_t phPin, turbPin, tdsPin;

    // Calibration parameters
    float phSlope, phIntercept;
    float turbSlope, turbIntercept;
    float tdsKValue;

    // pH calibration
    float phVoltages[2];
    float phValues[2];
    int phSampleCount;
    bool inPHCalib;
    void loadPHCalibration();
    void savePHCalibration();
    void calcPHRegression();

    // Turbidity calibration
    float turbVoltages[2];
    float turbValues[2];
    int turbSampleCount;
    bool inTurbCalib;
    void loadTurbCalibration();
    void saveTurbCalibration();
    void calcTurbRegression();

    // TDS calibration interactive
    bool inTDSCalib;
    bool tdsCalibReady;
    float tdsVoltage;
    float tdsEc;
    float temperature;

    void loadTDSCalibration();
    void saveTDSCalibration(float k);
    float readVoltage(uint8_t pin);

    // TDS calibration support
    byte parseTDSCommand(const String& cmd, float& calVal);
};

#endif
