#include <vector>
#include <stdint.h>

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

struct Pulse
{
    uint16_t intensity;
    unsigned long time;

    bool isIdle () {
        return intensity == 0;
    }

    bool operator==(const Pulse p)
    {
        return intensity == p.intensity &&
               time == p.time;
    }

    bool operator!=(const Pulse p)
    {
        return intensity != p.intensity ||
               time != p.time;
    }
};

class Heartbeat
{
public:
    void recordPulse(Pulse pulse);
    bool empty();
    std::vector<Pulse> pulses();

private:
    bool _isEqualToLastPulse (Pulse pulse);

    std::vector<Pulse> _pulses;
};

#endif