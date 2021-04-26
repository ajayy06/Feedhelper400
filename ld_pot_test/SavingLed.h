#ifndef SavingLed_h
#define SavingLed_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

typedef struct Colour {
    int red;
    int green;
    int blue;
} Colour;

class SavingLed
{
    public:
        SavingLed(int r_pin, int g_pin, int b_pin);
        void TestRGB();
    private:

        // colour pins
        int r_;
        int g_;
        int b_;

        void off();
        
};

#endif  // SavingLed.h