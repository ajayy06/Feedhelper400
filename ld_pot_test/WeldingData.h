#ifndef WeldingData_h
#define WeldingData_h

#include "curveFitting.h"
#include "EEPROM.h"

const int MAX_STORED_VALUES = 30;  // Don't go much over 100 - might stop working :D

// Maximum values
const double MAX_VOLTAGE = 10.0;
const double MAX_FEED = 18.0;

class WeldingData
{
    public:
        WeldingData();
        void addValues(double voltage, double feed);
        /**
         * \brief Get minimum displayed voltage
         * \return Minimum voltage to display as int, multiplied by 10
         **/
        int getMinDispVoltageInt();
        int getMaxDispVoltageInt();
        double getFeed(double voltage);
        void initEEPROM();
        void fitCurveToData();
    private:
        void writeInitialData();
        double solveSecondDegreeNumeric(double y_value);
        int data_points_;
        double min_displayed_voltage_;
        double max_displayed_voltage_;
        double coeffs_[3];
};

#endif // WeldingData.h