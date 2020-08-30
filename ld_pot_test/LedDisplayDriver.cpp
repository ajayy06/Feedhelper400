#include "LedDisplayDriver.h"

LedDisplayDriver::LedDisplayDriver(int din_pin, int cs_pin, int clk_pin):
    ld_(DigitLedDisplay(din_pin, cs_pin, clk_pin))
{
    
    // Brightness
    ld_.setBright(10);

    // Digit count
    ld_.setDigitLimit(8);
}

void LedDisplayDriver::displayDigit(byte address, int val, bool decimalpoint) {
    byte tableValue;
    if (!decimalpoint) {
        tableValue = pgm_read_byte_near(charTable + val);
    } else {
        tableValue = pgm_read_byte_near(charTable + val) | B10000000;
    }

	ld_.write(address, tableValue);
}

void LedDisplayDriver::displayValues(double voltage, double feed) {
    int voltage_int = voltage * 10;
    int feed_int = feed * 10;
    ld_.clear();

    int voltage_digits[3];
    voltage_digits[0] = voltage_int / 100;
    voltage_int -= voltage_digits[0] * 100;
    voltage_digits[1] = voltage_int / 10;
    voltage_digits[2] = voltage_int % 10;

    int feed_digits[3];
    feed_digits[0] = feed_int / 100;
    feed_int -= feed_digits[0] * 100;
    feed_digits[1] = feed_int / 10;
    feed_digits[2] = feed_int % 10;

    int current_addr = 1;
    for (int i = 2; i >= 0; i--) {

    if (voltage_digits[i] == 10) {
        voltage_digits[i] = 0;
    }

    if (feed_digits[i] == 10) {
        feed_digits[i] = 0;
    }

    if (current_addr == 2) {
        displayDigit(current_addr + 4, voltage_digits[i], true);
        delay(1);
        displayDigit(current_addr, feed_digits[i], true);
    } else {
        if (!(i == 0 && voltage_digits[i] == 0)) {
        displayDigit(current_addr + 4, voltage_digits[i]);
        }
        delay(1);
        if (!(i == 0 && feed_digits[i] == 0)) {
        displayDigit(current_addr, feed_digits[i]);
        }
    }
    current_addr++;
    }
}

void LedDisplayDriver::bootAnimation(int delay_ms) {
    // Boot animation
    for (int i = 1; i < 9; i++) {
        ld_.write(i, B01001000);
        delay(delay_ms);
        ld_.clear();
    }

    ld_.write(8, B00000110);
    delay(delay_ms);
    ld_.clear();

    for (int i = 8; i > 0; i--) {
        ld_.write(i, B00000001);
        delay(delay_ms);
        ld_.clear();
    }

    for (int i = 1; i < 9; i++) {
        ld_.write(i, B00000001);
        delay(delay_ms);
        ld_.clear();
    }

    ld_.write(8, B00000110);
    delay(delay_ms);
    ld_.clear();

    for (int i = 8; i > 0; i--) {
        ld_.write(i, B01001000);
        delay(delay_ms);
        ld_.clear();
    }

    ld_.write(1, B00110000);
    delay(delay_ms);
    ld_.clear();
}

void LedDisplayDriver::setDisplayBrightness(int brightness) {
    ld_.setBright(brightness);
}