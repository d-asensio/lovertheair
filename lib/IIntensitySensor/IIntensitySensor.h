#ifndef I_INTENSITY_SENSOR_H
#define I_INTENSITY_SENSOR_H

class IIntensitySensor {
public:
    virtual ~IIntensitySensor() = default;
    virtual void setup() = 0;
    virtual uint16_t read() = 0;
    virtual bool isAvailable() = 0;
};

#endif
