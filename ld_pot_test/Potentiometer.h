#ifndef Potentiometer_h
#define Potentiometer_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

const int ROLLING_AVG_READINGS = 20;
const int HYSTERESIS_AMOUNT = 10;


class Potentiometer
{
    public:
        Potentiometer(int analog_input_pin);
        int readValue();
    private:
        int rollingAverage();
        void hysteresis(int amount);
        int input_pin_;
        // Rolling average
        int readings_[ROLLING_AVG_READINGS];
        int read_index_;
        uint32_t total_;
        int average_;

        // Hysteresis
        int avg_ub_;
        int avg_lb_;
        int filtered_avg_;
};

#endif // Potentiometer.h