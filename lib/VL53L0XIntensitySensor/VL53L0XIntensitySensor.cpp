#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
 #include <EMA.h>

#include "VL53L0XIntensitySensor.h"

void VL53L0XIntensitySensor::setup()
{
    _initSuccess = _sensor.begin();
}

uint16_t VL53L0XIntensitySensor::read()
{
    if (!isAvailable())
    {
        return 0;
    }

    VL53L0X_RangingMeasurementData_t measure;

    _sensor.rangingTest(&measure, false);

    if (measure.RangeStatus == 4)
    {
        return 0;
    }

    if (measure.RangeMilliMeter > 500)
    {
        return 0;
    }

    static EMA<2> EMA_filter(255);

    uint16_t constrainedMillimeters = constrain(measure.RangeMilliMeter, 100, 400);
    uint16_t intensity = map(constrainedMillimeters, 100, 400, 255, 0);

    return EMA_filter(intensity);
}

bool VL53L0XIntensitySensor::isAvailable()
{
    return _initSuccess;
}