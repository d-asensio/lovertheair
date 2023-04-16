#include <vector>
#include <stdint.h>

#ifndef HEARTBEAT_H
#define HEARTBEAT_H

struct Pulse
{
    uint16_t intensity;
    unsigned long timestamps_millis;

    bool isIdle () {
        return intensity == 0;
    }

    bool operator==(const Pulse p)
    {
        return intensity == p.intensity &&
               timestamps_millis == p.timestamps_millis;
    }

    bool operator!=(const Pulse p)
    {
        return intensity != p.intensity ||
               timestamps_millis != p.timestamps_millis;
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