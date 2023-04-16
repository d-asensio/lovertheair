#include <functional>
#include "IClock.h"
#include "IIntensitySensor.h"
#include "Heartbeat.h"

class HeartbeatReader {
public:
    HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService);

    void loop();
    void setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback);
private:
    IClock* _clockService = nullptr;
    IIntensitySensor* _intensitySensorService = nullptr;

    Heartbeat* _current_heartbeat = nullptr;
    Pulse _previous_pulse = {0, 0};
    Pulse _last_recorded_pulse = {0, 0};

    std::function<void(Heartbeat*)> _callback;
};
