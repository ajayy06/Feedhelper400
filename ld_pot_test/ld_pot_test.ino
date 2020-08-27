// 8 Digit, 7-segment display first test
// Uses DigitLedDisplay library

#include "LedDisplayDriver.h"
#include "WeldingData.h"

// Pins
const int voltage_pot_pin = 7;
const int CLK = 5;
const int CS = 6;
const int DIN = 7;

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
float old_voltage = 10.1;

// Final parameters
float voltage = 0;
float feed = 0;

int BOOT_DELAY = 30;

LedDisplayDriver display = LedDisplayDriver(DIN, CS, CLK);
WeldingData data = WeldingData();


void setup() {
  Serial.begin(115200);
  pinMode(12, INPUT_PULLUP);
  if (digitalRead(12) == LOW) {
    EEPROM.write(1000, 'R');
  }

  data.initEEPROM();

  // Init rolling average array to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  data.fitCurveToData();
  display.bootAnimation(BOOT_DELAY);
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

  // DISPLAYED VOLTAGE
  int int_min_voltage = data.getMinDispVoltageInt();
  int int_max_voltage = data.getMaxDispVoltageInt();

  voltage = map(filtered_avg, 10, 1010, int_min_voltage, int_max_voltage) / 10.0;
  if (voltage > int_max_voltage / 10.0) {
    voltage = int_max_voltage / 10.0;
  }

  if (voltage < int_min_voltage / 10.0) {
    voltage = int_min_voltage / 10.0;
  }


  if (delay_time > 20 && voltage != old_voltage) {
    old_voltage = voltage;
    feed = data.getFeed(voltage);
    Serial.print("voltage: ");
    Serial.println(voltage);
    display.displayValues(voltage, feed);
    delay_time = 0;
  }
  delay_time++;
  delay(1);
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