#ifndef LedDisplayDriver_h
#define LedDisplayDriver_h

#include "DigitLedDisplay.h"

class LedDisplayDriver
{
    public:
        LedDisplayDriver(int din_pin, int cs_pin, int clk_pin);
        void bootAnimation(int delay_ms);
        void displayValues(float voltage, float feed);
    private:
        void displayDigit(byte address, int val, bool decimalpoint = false);
        DigitLedDisplay ld_;
};

#endif  // LedDisplayDriver.h