#ifndef I_INTENSITY_SENSOR_H
#define I_INTENSITY_SENSOR_H

class IIntensitySensor {
public:
    virtual uint16_t read() = 0;
};

#endif
