#include "Heartbeat.h"

void Heartbeat::recordPulse(Pulse pulse) {
   if (_isEqualToLastPulse(pulse)) {
      return;
   }

   _pulses.push_back(pulse);
}

bool Heartbeat::empty() {
   return _pulses.empty();
}

std::vector<Pulse> Heartbeat::pulses() {
    return _pulses;
}

bool Heartbeat::_isEqualToLastPulse(Pulse pulse) {
   return _pulses.size() > 0 && _pulses.back() == pulse;
}
