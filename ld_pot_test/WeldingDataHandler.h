#ifndef WeldingDataHandler_h
#define WeldingDataHandler_h

const int STORAGE_ARRAY_SIZE = 30;

class WeldingDataHandler
{
    public:
        WeldingDataHandler();
        void addValues(float voltage, float feed);
        int getMinDispVoltage();
        int getMaxDispVoltage();
    private:
        float voltages_[STORAGE_ARRAY_SIZE];
        float feeds_[STORAGE_ARRAY_SIZE];
        int data_points_;
        float min_displayed_voltage_;
        float max_displayed_voltage_;
};

#endif // WeldingDataHandler.h