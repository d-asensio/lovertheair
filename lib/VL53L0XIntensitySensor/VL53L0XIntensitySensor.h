#include <stdint.h>
#include <Adafruit_VL53L0X.h>

#include "IIntensitySensor.h"

#ifndef VL53L0X_INTENSITY_SENSOR_H
#define VL53L0X_INTENSITY_SENSOR_H

class VL53L0XIntensitySensor : public IIntensitySensor {
public:
    virtual void setup();
    virtual uint16_t read();
    virtual bool isAvailable();

private:
    Adafruit_VL53L0X _sensor = Adafruit_VL53L0X();
    bool _initSuccess = false;
};

#endif
