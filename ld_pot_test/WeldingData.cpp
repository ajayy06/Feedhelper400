#include "WeldingData.h"

WeldingData::WeldingData():
    data_points_(0),
    min_displayed_voltage_(0),
    max_displayed_voltage_(0) {
}

void WeldingData::addValues(float voltage, float feed) {
    // TODO
}

int WeldingData::getMinDispVoltageInt() {
    return (int)min_displayed_voltage_ * 10;
}

int WeldingData::getMaxDispVoltageInt() {
    return (int)max_displayed_voltage_ * 10;
}

void WeldingData::fitCurveToData() {
    double voltages[data_points_];
    double feeds[data_points_];

    for (int i = 0; i < data_points_; i++) {
        int eeprom_addr = (i + 1) * sizeof(double);
        voltages[i] = EEPROM.read(eeprom_addr);
        Serial.print("read voltage is: ");
        Serial.println(voltages[i]);
        feeds[i] = EEPROM.read(eeprom_addr + MAX_STORED_VALUES * sizeof(double));
    }

    int fit_return_value = fitCurve(2, data_points_, voltages, feeds, sizeof(coeffs_)/sizeof(double), coeffs_);

    float A = coeffs_[0];
    float B = coeffs_[1];
    float C = coeffs_[2];
    float C_eqn1 = C - MAX_FEED;

    // Solve equation fit == MAX_FEED
    // --> Get the voltage with maximum feed
    max_displayed_voltage_ = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn1))/(2*A);

    // Get an approximate of the function min value.
    // The fit is probably inaccurate with values near the min....
    float tmp_feed = 100;
    float min_feed = 100;

    for (int i = 0; i < max_displayed_voltage_ * 10; i++) {
        tmp_feed = getFeed(i / 10.0);
        if (tmp_feed < min_feed) {
            min_feed = tmp_feed;
        }
    }

    // If this happens, the fit might be wrong...
    if (min_feed < 0) {
        min_feed = 0;
    }

    float C_eqn2 = C - min_feed;

    min_displayed_voltage_ = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn2))/(2*A);

    Serial.println(fit_return_value);
    Serial.println(coeffs_[0]);
    Serial.println(coeffs_[1]);
    Serial.println(coeffs_[2]);
}

void WeldingData::writeInitialData() {
    double initial_voltages[7] = {  3.4,
                        4.0,
                        4.4,
                        4.8,
                        5.1,
                        5.2,
                        5.3
                    };

    double initial_feeds[7] = { 6.0,
                    7.0,
                    9.0,
                    12.0,
                    13.0,
                    14.0,
                    15.0
                };

    Serial.println("writeInitialData");

    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
    }

    data_points_ = 7;
    for (int i = 0; i < data_points_; i++) {
        int eeprom_addr = (i + 1) * sizeof(double);
        EEPROM.write(eeprom_addr, initial_voltages[i]);  // addresses 4 to MAX_STORED_VALUES
        EEPROM.write(eeprom_addr + MAX_STORED_VALUES * sizeof(double), initial_feeds[i]); // addresses 4 + MAX_STORED_VALUES to that + MAX_STORED_VALUES
    }

    EEPROM.write(1000, 'I');

    fitCurveToData();
}

float WeldingData::getFeed(float voltage) {
  return coeffs_[0] * pow(voltage, 2) + coeffs_[1] * voltage + coeffs_[2];
}

void WeldingData::initEEPROM() {
    char eeprom_reset_flag = EEPROM.read(1000);
    if (eeprom_reset_flag == 'R') {
        writeInitialData();
    }
}