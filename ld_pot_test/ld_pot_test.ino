// 8 Digit, 7-segment display first test
// Uses DigitLedDisplay library

#include "LedDisplayDriver.h"
#include "WeldingData.h"
#include "Potentiometer.h"

#include <elapsedMillis.h>

const int COOLDOWN_AMOUNT = 20;

// Pins
const int feed_pot_pin = A6;
const int voltage_pot_pin = A7;
const int CLK = 5;
const int CS = 6;
const int DIN = 7;

// Additional optimization variables
uint8_t delay_time = 0;
float old_voltage = 10.1;

volatile bool button_pressed = false;
unsigned button_cooldown = 20;
unsigned release_cooldown = 20;

// Final parameters
float voltage = 0;
float feed = 0;

int BOOT_DELAY = 30;

LedDisplayDriver display = LedDisplayDriver(DIN, CS, CLK);
WeldingData data = WeldingData();
Potentiometer voltage_pot = Potentiometer(voltage_pot_pin);
Potentiometer feed_pot = Potentiometer(feed_pot_pin);


void setup() {
  Serial.begin(115200);

  pinMode(12, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(voltage_pot_pin, INPUT);
  pinMode(feed_pot_pin, INPUT);

  attachInterrupt(digitalPinToInterrupt(2), inputValue, FALLING);

  if (digitalRead(12) == LOW) {
    EEPROM.write(1000, 'R');
  }

  data.initEEPROM();
  data.fitCurveToData();

  display.bootAnimation(BOOT_DELAY);
}

void loop() {

  incrementCooldowns();


  if (button_pressed) {  // Enter saving mode
    savingMode();
  }


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

void inputValue() {
  if (button_cooldown < COOLDOWN_AMOUNT || release_cooldown < COOLDOWN_AMOUNT) {
    return;
  }

  button_cooldown = 0;
  Serial.println("Interrupt trigggggerereddddd");
  button_pressed = true;
}

void incrementCooldowns() {
  // ==========  BUTTON COOLDOWN S***  ==========
  if (button_cooldown < COOLDOWN_AMOUNT) {
    button_cooldown++;
  }
  if (digitalRead(2) == LOW) {
    release_cooldown = 0;
  } else {
    if (release_cooldown < COOLDOWN_AMOUNT) {
      release_cooldown++;
    }
  }
  // ==========  /BUTTON COOLDOWN S*** ==========
}

void savingMode() {
  button_pressed = false;

  elapsedMillis stopwatch;
  stopwatch = 0;
  display.setDisplayBrightness(1);
  bool dim = true;

  double voltage_to_save = voltage;
  double feed_to_save = feed;

  double prev_voltage = voltage;
  double prev_feed = feed;

  while ( true ) {  // Saving mode loop
    incrementCooldowns();

    // Flashing the brightness
    if (stopwatch > 500) {
      if (dim) {
        display.setDisplayBrightness(10);
        dim = false;
      } else {
        display.setDisplayBrightness(1);
        dim = true;
      }
      stopwatch = 0;
    }
    
    if (button_pressed) {  // Exit saving mode if button is pressed again
      button_pressed = false;
      display.setDisplayBrightness(10);
      break;
    }

    if (voltage_to_save != prev_voltage || feed_to_save != prev_feed) {
      display.displayValues(voltage_to_save, feed_to_save);
      prev_voltage = voltage_to_save;
      prev_feed = feed_to_save;
    }

    voltage_to_save = map(voltage_pot.readValue(), 10, 1010, 0, 100) / 10.0;
    feed_to_save = map(feed_pot.readValue(), 10, 1010, 0, 180) / 10.0;
  }

  display.displayValues(voltage, feed);
  // Exit the saving mode
}
