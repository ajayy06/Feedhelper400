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

enum colourlist {LED_PURPLE, LED_RED, LED_GREEN, LED_YELLOW};

class SavingLed
{
    public:
        SavingLed(int r_pin, int g_pin, int b_pin);
        void TestRGB();
        void ShowColour(colourlist colour_to_show);
        void DimAnimation(int delay_ms);
        void off();
    private:

        // colour pins
        int r_;
        int g_;
        int b_;

        // current values
        int r_value_;
        int g_value_;
        int b_value_;

        
};

#endif  // SavingLed.h