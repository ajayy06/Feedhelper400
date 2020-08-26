#include "WeldingDataHandler.h"

WeldingDataHandler::WeldingDataHandler():
    data_points_(0) {

}

void WeldingDataHandler::addValues(float voltage, float feed) {
    // TODO
}

int WeldingDataHandler::getMinDispVoltage() {
    return (int)min_displayed_voltage_ * 10;
}

int WeldingDataHandler::getMaxDispVoltage() {
    return (int)max_displayed_voltage_ * 10;
}