#include "WeldingData.h"

WeldingData::WeldingData():
    data_points_(0),
    min_displayed_voltage_(0),
    max_displayed_voltage_(0) {
}

void WeldingData::addValues(double voltage, double feed) {
    data_points_++;  // Data point validity check missing!!
    EEPROM.write(0, data_points_);

    int eeprom_addr = data_points_ * sizeof(double);
    EEPROM.put(eeprom_addr, voltage);
    EEPROM.put(eeprom_addr + MAX_STORED_VALUES * sizeof(double), feed);
}

int WeldingData::getMinDispVoltageInt() {
    int val = min_displayed_voltage_ * 10;
    return val;
}

int WeldingData::getMaxDispVoltageInt() {
    int val = max_displayed_voltage_ * 10;
    return val;
}

void WeldingData::fitCurveToData() {
    data_points_ = EEPROM.read(0);
    double voltages[data_points_];
    double feeds[data_points_];

    for (int i = 0; i < data_points_; i++) {
        int eeprom_addr = (i + 1) * sizeof(double);
        EEPROM.get(eeprom_addr, voltages[i]);
        EEPROM.get(eeprom_addr + MAX_STORED_VALUES * sizeof(double), feeds[i]);
    }

    int fit_return_value = fitCurve(2, data_points_, voltages, feeds, sizeof(coeffs_)/sizeof(double), coeffs_);

    double A = coeffs_[0];  // ok
    double B = coeffs_[1];
    double C = coeffs_[2];
    double C_eqn1 = C - MAX_FEED;


    // Solve equation fit == MAX_FEED
    // --> Get the voltage with maximum feed
    //max_displayed_voltage_ = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn1))/(2*A);
    max_displayed_voltage_ = solveSecondDegreeNumeric(MAX_FEED);

    Serial.print("maximum displayed voltage = ");
    Serial.println(max_displayed_voltage_);

    // Get an approximate of the function min value.
    // The fit is probably inaccurate with values near the min....
    double tmp_feed = 100;
    double min_feed = 100;

    for (int i = 0; i < max_displayed_voltage_ * 10; i++) {
        tmp_feed = getFeed(i / 10.0);
        if (tmp_feed < min_feed) {
            min_feed = tmp_feed;
        }
    }

    Serial.print("min_feed = ");
    Serial.println(min_feed);

    // If this happens, the fit might be wrong...
    if (min_feed < 0) {
        min_feed = 0;
    }

    float C_eqn2 = C - min_feed;

    //min_displayed_voltage_ = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn2))/(2*A);
    min_displayed_voltage_ = solveSecondDegreeNumeric(min_feed);

    Serial.print("minimum displayed voltage = ");
    Serial.println(min_displayed_voltage_);

    Serial.print("2nd order coefficient: ");
    Serial.println(coeffs_[0]);
    Serial.print("1st order coefficient: ");
    Serial.println(coeffs_[1]);
    Serial.print("constant coefficient: ");
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
        Serial.println("Writing to EEPROM...");
        EEPROM.put(eeprom_addr, initial_voltages[i]);  // addresses 4 to MAX_STORED_VALUES
        EEPROM.put(eeprom_addr + MAX_STORED_VALUES * sizeof(double), initial_feeds[i]); // addresses 4 + MAX_STORED_VALUES to that + MAX_STORED_VALUES
    }

    EEPROM.write(1000, 'I');  // Unset reset flag
    EEPROM.write(0, data_points_);  // Number of data points is stored in the beginning
}

double WeldingData::getFeed(double voltage) {
  return coeffs_[0] * pow(voltage, 2) + coeffs_[1] * voltage + coeffs_[2];
}

void WeldingData::initEEPROM() {
    char eeprom_reset_flag = EEPROM.read(1000);
    if (eeprom_reset_flag == 'R') {
        writeInitialData();
    }
}

double WeldingData::solveSecondDegreeNumeric(double y_value) {
    double imprecision = 1000.0; // approximate will be better than this...
    double prev_imprecision = 1000.0;
    double final_val;
    double current_val;
    for (double i = 20; i > 0; i = i - 0.01) {
        current_val = getFeed(i);
        imprecision = abs(current_val - y_value);
        if (imprecision > prev_imprecision) {
            Serial.print("i = ");
            Serial.println(i);
            Serial.print("imprecision = ");
            Serial.println(imprecision);
            break;
        } else {
            prev_imprecision = imprecision;
        }
        final_val = i;
    }

    return final_val;
}