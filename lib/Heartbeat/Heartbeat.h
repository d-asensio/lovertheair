#include <vector>
#include <stdint.h>

struct Pulse
{
    uint16_t intensity;
    unsigned int timestamps_millis;

    bool operator==(const Pulse p)
    {
        return intensity == p.intensity &&
               timestamps_millis == p.timestamps_millis;
    }
};

class Heartbeat
{
public:
    void recordPulse(Pulse pulse);
    bool empty();
    std::vector<Pulse> pulses();

private:
    std::vector<Pulse> _pulses;
};