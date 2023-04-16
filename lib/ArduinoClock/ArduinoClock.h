#include "IClock.h"

#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

class ArduinoClock : public IClock {
public:
  virtual unsigned long milliseconds();
};

#endif