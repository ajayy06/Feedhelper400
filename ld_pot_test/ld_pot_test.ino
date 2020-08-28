// 8 Digit, 7-segment display first test
// Uses DigitLedDisplay library

#include "LedDisplayDriver.h"
#include "WeldingData.h"
#include "Potentiometer.h"

// Pins
const int voltage_pot_pin = A7;
const int CLK = 5;
const int CS = 6;
const int DIN = 7;

// Additional optimization variables
uint8_t delay_time = 0;
float old_voltage = 10.1;

// Final parameters
float voltage = 0;
float feed = 0;

int BOOT_DELAY = 30;

LedDisplayDriver display = LedDisplayDriver(DIN, CS, CLK);
WeldingData data = WeldingData();
Potentiometer voltage_pot = Potentiometer(voltage_pot_pin);


void setup() {
  Serial.begin(115200);
  pinMode(12, INPUT_PULLUP);
  pinMode(voltage_pot_pin, INPUT);

  if (digitalRead(12) == LOW) {
    EEPROM.write(1000, 'R');
  }

  data.initEEPROM();

  data.fitCurveToData();
  display.bootAnimation(BOOT_DELAY);
}

void loop() {

  // DISPLAYED VOLTAGE
  int int_min_voltage = data.getMinDispVoltageInt();
  int int_max_voltage = data.getMaxDispVoltageInt();

  int voltage_pot_value = voltage_pot.readValue();

  voltage = map(voltage_pot_value, 10, 1010, int_min_voltage, int_max_voltage) / 10.0;
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