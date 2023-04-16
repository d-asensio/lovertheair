#include <functional>
#include "Heartbeat.h"

class IClock {
public:
    virtual unsigned int millis() = 0;
};

class IIntensitySensor {
public:
    virtual uint16_t read() = 0;
};

class HeartbeatReader {
public:
    HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService);

    void loop();
    void setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback);
private:
    IClock* _clockService = nullptr;
    IIntensitySensor* _intensitySensorService = nullptr;

    Heartbeat* _current_heartbeat = nullptr;
    Pulse _last_pulse = {0, 0};

    std::function<void(Heartbeat*)> _callback;
};
