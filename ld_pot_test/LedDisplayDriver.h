#ifndef LedDisplayDriver_h
#define LedDisplayDriver_h

#include "DigitLedDisplay.h"
#include "SavingLed.h"

class LedDisplayDriver
{
    public:
        LedDisplayDriver(int din_pin, int cs_pin, int clk_pin);
        void bootAnimation(int delay_ms);
        void displayValues(double voltage, double feed);
        void setDisplayBrightness(int brightness);
        void notSavedAnimation(int delay_ms, SavingLed *led = nullptr);
        void savedAnimation(int delay_ms, SavingLed *led = nullptr);
        void on();
        void off();
    private:
        void displayDigit(byte address, int val, bool decimalpoint = false);
        DigitLedDisplay ld_;
};

#endif  // LedDisplayDriver.h