#ifndef WeldingData_h
#define WeldingData_h

#include "curveFitting.h"
#include "EEPROM.h"

const int MAX_STORED_VALUES = 30;  // Don't go much over 100 - might stop working :D

// Maximum values
const float MAX_VOLTAGE = 10.0;
const float MAX_FEED = 18.0;

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
        float getFeed(float voltage);
        void initEEPROM();
        void fitCurveToData();
    private:
        void writeInitialData();
        int data_points_;
        float min_displayed_voltage_;
        float max_displayed_voltage_;
        double coeffs_[3];
};

#endif // WeldingData.h