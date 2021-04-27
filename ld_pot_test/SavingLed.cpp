#include "SavingLed.h"

Colour SAVING_PURPLE = {
    180,
    10,
    70
};

Colour SAVING_RED = {
    255,
    0,
    0
};

Colour SAVING_GREEN = {
    10,
    255,
    0
};

Colour SAVING_YELLOW = {
    255,
    200,
    0
};

Colour colour_palette[4] = {SAVING_PURPLE, SAVING_RED, SAVING_GREEN, SAVING_YELLOW};



SavingLed::SavingLed(int r_pin, int g_pin, int b_pin):
    r_(r_pin),
    g_(g_pin),
    b_(b_pin),
    
    r_value_(0),
    g_value_(0),
    b_value_(0) {
        pinMode(r_pin, OUTPUT);
        pinMode(g_pin, OUTPUT);
        pinMode(b_pin, OUTPUT);
}

void SavingLed::TestRGB() {
    analogWrite(r_, 255);
    analogWrite(g_, 255);
    analogWrite(b_, 255);
    delay(500);
    off();
}

void SavingLed::ShowColour(colourlist colour_to_show) {
    analogWrite(r_, colour_palette[colour_to_show].red);
    analogWrite(g_, colour_palette[colour_to_show].green);
    analogWrite(b_, colour_palette[colour_to_show].blue);

    r_value_ = colour_palette[colour_to_show].red;
    g_value_ = colour_palette[colour_to_show].green;
    b_value_ = colour_palette[colour_to_show].blue;
}


void SavingLed::off() {
    analogWrite(r_, 0);
    analogWrite(g_, 0);
    analogWrite(b_, 0);
}

void SavingLed::DimAnimation(int delay_ms) {
    for (float i = 1.0; i >= 0; i -= 0.01) {
        analogWrite(r_, r_value_ * i);
        analogWrite(g_, g_value_ * i);
        analogWrite(b_, b_value_ * i);

        delay(delay_ms);
    }

    r_value_ = 0;
    g_value_ = 0;
    b_value_ = 0;
    
}