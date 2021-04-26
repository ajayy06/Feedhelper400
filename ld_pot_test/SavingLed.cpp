#include "SavingLed.h"

Colour SAVING_PURPLE = {
    255,
    10,
    100
};

Colour SAVING_RED = {
    255,
    0,
    0
};

Colour SAVING_GREEN = {
    10,
    255,
    10
};



SavingLed::SavingLed(int r_pin, int g_pin, int b_pin):
    r_(r_pin),
    g_(g_pin),
    b_(b_pin) {
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


void SavingLed::off() {
    analogWrite(r_, 0);
    analogWrite(g_, 0);
    analogWrite(b_, 0);
}