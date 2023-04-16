#include "Heartbeat.h"

void Heartbeat::recordPulse(Pulse pulse) {
   _pulses.push_back(pulse);
}

bool Heartbeat::empty() {
   return _pulses.empty();
}

std::vector<Pulse> Heartbeat::pulses() {
    return _pulses;
}