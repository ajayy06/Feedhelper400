#include "Potentiometer.h"

Potentiometer::Potentiometer(int analog_input_pin):
    input_pin_(analog_input_pin),
    read_index_(0),
    total_(0),
    average_(0),
    avg_ub_(0),
    avg_lb_(0),
    filtered_avg_(0) {
        // Init rolling average array to 0
        for (int i = 0; i < ROLLING_AVG_READINGS; i++) {
            readings_[i] = 0;
        }
}

int Potentiometer::rollingAverage() {
  // subtract the last reading:
  total_ = total_ - readings_[read_index_];
  // read from the sensor:
  readings_[read_index_] = analogRead(input_pin_);
  // add the reading to the total:
  total_ = total_ + readings_[read_index_];
  // advance to the next position in the array:
  read_index_ = read_index_ + 1;

  // if we're at the end of the array...
  if (read_index_ >= ROLLING_AVG_READINGS) {
    // ...wrap around to the beginning:
    read_index_ = 0;
  }

  // calculate the average:
  return total_/ROLLING_AVG_READINGS;
}

void Potentiometer::hysteresis(int amount) {
    avg_ub_ = average_ + amount;
    avg_lb_ = average_ - amount;

    if (filtered_avg_ < avg_lb_) {
        filtered_avg_ = avg_lb_;
    } else if (filtered_avg_ > avg_ub_) {
        filtered_avg_ = avg_ub_;
    }
}

int Potentiometer::readValue() {
    average_ = rollingAverage();
    hysteresis(HYSTERESIS_AMOUNT);
    return filtered_avg_;
}