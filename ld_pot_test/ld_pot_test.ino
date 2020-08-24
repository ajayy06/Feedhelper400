// 8 Digit, 7-segment display first test
// Uses DigitLedDisplay library

#include "DigitLedDisplay.h"
#include "curveFitting.h"

#include "initial_data.h"

// Pins
int voltage_pot_pin = 7;
int CLK = 5;
int CS = 6;
int DIN = 7;

// Create new instance of DigitLedDisplay class
DigitLedDisplay ld = DigitLedDisplay(DIN, CS, CLK);

// For rolling avg
const int numReadings = 20;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
uint32_t total = 0;             // the running total
int average = 0;                // the average

// For hysteresis
int avg_ub = 0;
int avg_lb = 0;
int filtered_avg;

// Additional optimization variables
uint8_t delay_time = 0;
int old_voltage = 101;

// Final parameters
int voltage = 0;
int feed = 0;

int BOOT_DELAY = 30;


void setup() {
  Serial.begin(115200);

  // Init rolling average array to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
  // Brightness
  ld.setBright(15);

  // Digit count
  ld.setDigitLimit(8);

  bootAnimation(BOOT_DELAY);

  ld.clear();
  delay(BOOT_DELAY);
}

void loop() {
  average = rollingAverage(voltage_pot_pin);

  // -HYSTERESIS
  avg_ub = average + 10;
  avg_lb = average - 10;

  if (filtered_avg < avg_lb) {
    filtered_avg = avg_lb;
  } else if (filtered_avg > avg_ub) {
    filtered_avg = avg_ub;
  }

  // CALCULATE THE VOLTAGE
  voltage = map(filtered_avg, 10, 1010, 0, 100);
  if (voltage > 100) {
    voltage = 100;
  }

  if (voltage < 0) {
    voltage = 0;
  }


  if (delay_time > 20 && voltage != old_voltage) {
    old_voltage = voltage;
    int feed = calculateFeed(voltage);
    updateDisplay(voltage, feed);
    delay_time = 0;
  }
  delay_time++;
  delay(1);
}


void updateDisplay(int voltage, int feed) {
  ld.clear();

  int voltage_digits[3];
  voltage_digits[0] = voltage / 100;
  voltage -= voltage_digits[0] * 100;
  voltage_digits[1] = voltage / 10;
  voltage_digits[2] = voltage % 10;

  int feed_digits[3];
  feed_digits[0] = feed / 100;
  feed -= feed_digits[0] * 100;
  feed_digits[1] = feed / 10;
  feed_digits[2] = feed % 10;

  int current_addr = 1;
  for (int i = 2; i >= 0; i--) {
  
    if (voltage_digits[i] == 10) {
      voltage_digits[i] = 0;
    }

    if (feed_digits[i] == 10) {
      feed_digits[i] = 0;
    }

    if (current_addr == 2) {
      ld.tableWithPoint(current_addr + 4, voltage_digits[i]);
      delay(1);
      ld.tableWithPoint(current_addr, feed_digits[i]);
    } else {
      if (!(i == 0 && voltage_digits[i] == 0)) {
        ld.table(current_addr + 4, voltage_digits[i]);
      }
      delay(1);
      if (!(i == 0 && feed_digits[i] == 0)) {
        ld.table(current_addr, feed_digits[i]);
      }
    }
    current_addr++;
  }
}

int calculateFeed(int voltage) {
  return voltage * 18 / 10;
}

int rollingAverage(const int analog_input_pin) {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(analog_input_pin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  return total/numReadings;
}

void bootAnimation(int delay_ms) {
  // Boot animation
  for (int i = 1; i < 9; i++) {
    ld.write(i, B01001000);
    delay(delay_ms);
    ld.clear();
  }

  ld.write(8, B00000110);
  delay(delay_ms);
  ld.clear();

  for (int i = 8; i > 0; i--) {
    ld.write(i, B00000001);
    delay(delay_ms);
    ld.clear();
  }

  for (int i = 1; i < 9; i++) {
    ld.write(i, B00000001);
    delay(delay_ms);
    ld.clear();
  }

  ld.write(8, B00000110);
  delay(delay_ms);
  ld.clear();

  for (int i = 8; i > 0; i--) {
    ld.write(i, B01001000);
    delay(delay_ms);
    ld.clear();
  }

  ld.write(1, B00110000);
  delay(delay_ms);
  ld.clear();
}
