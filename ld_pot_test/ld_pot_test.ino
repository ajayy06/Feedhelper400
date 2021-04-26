// 8 Digit, 7-segment display first test
// Uses DigitLedDisplay library

/**
 * TODO:
 * 
 * Test the s*** out of saving different values, check with matlab that the
 * curve fitting does not piss in your morning cereal
 */

#include "LedDisplayDriver.h"
#include "WeldingData.h"
#include "Potentiometer.h"
#include "SavingLed.h"

#include <elapsedMillis.h>

const int COOLDOWN_AMOUNT = 20;

// Pins
const int feed_pot_pin = A6;
const int voltage_pot_pin = A7;
const int CLK = 5;
const int CS = 6;
const int DIN = 7;

const int RED = 9;
const int GREEN = 3;
const int BLUE = 10;

// Additional optimization variables
uint8_t delay_time = 0;
double old_voltage = 10.1;
double old_feed = 18.1;

volatile bool button_pressed = false;
unsigned button_cooldown = 20;
unsigned release_cooldown = 20;

// Final parameters
double voltage = 0;
double feed = 0;

int BOOT_DELAY = 30;

int CONFIRMATION_FLASH_CYCLES = 10;
int SAVING_FLASH_CYCLES = 15;

LedDisplayDriver display = LedDisplayDriver(DIN, CS, CLK);
WeldingData data = WeldingData();
Potentiometer voltage_pot = Potentiometer(voltage_pot_pin);
Potentiometer feed_pot = Potentiometer(feed_pot_pin);
SavingLed led = SavingLed(RED, GREEN, BLUE);


void setup() {
  Serial.begin(115200);

  pinMode(12, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(voltage_pot_pin, INPUT);
  pinMode(feed_pot_pin, INPUT);

  attachInterrupt(digitalPinToInterrupt(2), inputValue, FALLING);

  if (digitalRead(12) == LOW) {  // Reset pin is connected to GND
    EEPROM.write(1000, 'R'); // Set the ("R")eset flag
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

  followCurve(false);

  delay(1);  // Stability
}

void followCurve(bool force) {
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


  if (voltage != old_voltage || feed != old_feed || force) {
    old_voltage = voltage;
    old_feed = feed;
    feed = data.getFeed(voltage);
    display.displayValues(voltage, feed);
  }
}

void inputValue() {  // Innterrupt function
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
  led.TestRGB();
  button_pressed = false;

  elapsedMillis stopwatch;
  stopwatch = 0;
  display.setDisplayBrightness(1);
  bool dim = true;

  double voltage_to_save = voltage;
  double feed_to_save = feed;

  double prev_voltage = voltage;
  double prev_feed = feed;

  bool saved;

  int cycles = 0;

  for ( ;; ) {  // Saving mode loop
    incrementCooldowns();

    // Flashing the brightness
    if (stopwatch > 500) {
      if (dim) {
        display.setDisplayBrightness(10);
        dim = false;
      } else {
        display.setDisplayBrightness(1);
        dim = true;
        cycles++;
      }
      stopwatch = 0;
    }

    if (cycles > SAVING_FLASH_CYCLES) {  // Exit without saving if not confirmed within x cycles
      break;
    }

    if (voltage_to_save != prev_voltage || feed_to_save != prev_feed) {
      cycles = 0;
      display.displayValues(voltage_to_save, feed_to_save);
      prev_voltage = voltage_to_save;
      prev_feed = feed_to_save;
    }

    voltage_to_save = map(voltage_pot.readValue(), 5, 1010, 0, 100) / 10.0;
    feed_to_save = map(feed_pot.readValue(), 5, 1010, 0, 180) / 10.0;

    Serial.println(feed_to_save);

    if (button_pressed) {  // Enter confirmation mode if button is pressed again
      button_pressed = false;
      display.setDisplayBrightness(10);
      saved = confirmationMode(voltage_to_save, feed_to_save);
      break;
    }
  }

  followCurve(true);

  if (saved) {
    display.savedAnimation(100);
  } else {
    display.notSavedAnimation(100);
  }

  // Exit the saving mode
}

bool confirmationMode(double voltage_to_save, double feed_to_save) {
  elapsedMillis stopwatch;
  stopwatch = 0;
  int cycles = 0;
  bool off = false;

  for ( ;; ) {  // confirmation mode loop
    incrementCooldowns();

    // Flashing the screen on and off
    if (stopwatch > 200) {
      if (off) {
        display.on();
        off = false;
      } else {
        display.off();
        off = true;
        cycles++;
      }
      stopwatch = 0;
    }

    if (cycles > CONFIRMATION_FLASH_CYCLES) {  // Exit without saving if not confirmed within x cycles
      display.on();
      return false;
    }

    if (button_pressed) {  // Save confirmed -> exit and actually save new values
      display.on();
      button_pressed = false;
      data.addValues(voltage_to_save, feed_to_save);
      data.fitCurveToData();
      return true;
    }

  }
}
