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
float old_voltage = 10.1;

// Final parameters
float voltage = 0;
float feed = 0;

int BOOT_DELAY = 30;

// For curve fitting
int order = 2;
double coeffs[3];

// Maximum values
float MAX_VOLTAGE = 10.0;
float MAX_FEED = 18.0;
float disp_max_voltage = 0;
float disp_min_voltage = 0;


void setup() {

  Serial.begin(115200);

  // Init rolling average array to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
  // Brightness
  ld.setBright(10);

  // Digit count
  ld.setDigitLimit(8);

  bootAnimation(BOOT_DELAY);

  ld.clear();
  delay(BOOT_DELAY);


  // FOR TESTING!!!111 REMOVE WHEN DONE
  fitCurveToData();
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
  int int_min_voltage = disp_min_voltage * 10;
  int int_max_voltage = disp_max_voltage * 10;

  voltage = map(filtered_avg, 10, 1010, int_min_voltage, int_max_voltage) / 10.0;
  if (voltage > disp_max_voltage) {
    voltage = disp_max_voltage;
  }

  if (voltage < disp_min_voltage) {
    voltage = disp_min_voltage;
  }


  if (delay_time > 20 && voltage != old_voltage) {
    old_voltage = voltage;
    feed = calculateFeed(voltage);
    updateDisplay(voltage, feed);
    delay_time = 0;
  }
  delay_time++;
  delay(1);
}

void displayDigit(byte address, int val, bool decimalpoint = false) {
	byte tableValue;
  if (!decimalpoint) {
    tableValue = pgm_read_byte_near(charTable + val);
  } else {
    tableValue = pgm_read_byte_near(charTable + val) | B10000000;
  }
	
	ld.write(address, tableValue);
}


void updateDisplay(float voltage, float feed) {
  int voltage_int = voltage * 10;
  int feed_int = feed * 10;
  ld.clear();

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

float calculateFeed(float voltage) {
  return coeffs[0] * pow(voltage, 2) + coeffs[1] * voltage + coeffs[2];
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

void fitCurveToData() {
  int fit_return_value = fitCurve(order, sizeof(voltages)/sizeof(double), voltages, feeds, sizeof(coeffs)/sizeof(double), coeffs);

  float A = coeffs[0];
  float B = coeffs[1];
  float C = coeffs[2];
  float C_eqn1 = C - MAX_FEED;

  // Solve equation fit == MAX_FEED
  // --> Get the voltage with maximum feed
  disp_max_voltage = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn1))/(2*A);

  // Get an approximate of the function min value.
  // The fit is probably inaccurate with values near the min....
  float tmp_feed = 100;
  float min_feed = 100;

  for (int i = 0; i < disp_max_voltage * 10; i++) {
    tmp_feed = calculateFeed(i / 10.0);
    if (tmp_feed < min_feed) {
      min_feed = tmp_feed;
    }
  }

  // If this happens, the fit might be wrong...
  if (min_feed < 0) {
    min_feed = 0;
  }

  float C_eqn2 = C - min_feed;

  disp_min_voltage = (-B + sqrt(pow(B, 2) - 4 * A * C_eqn2))/(2*A);

  Serial.println(fit_return_value);
  Serial.println(coeffs[0]);
  Serial.println(coeffs[1]);
  Serial.println(coeffs[2]);
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
