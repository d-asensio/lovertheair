#include <functional>

#include "IClock.h"
#include "IIntensitySensor.h"
#include "Heartbeat.h"

#ifndef HEARTBEAT_READER_H
#define HEARTBEAT_READER_H

class HeartbeatReader {
public:
    HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService);

    void loop();
    void setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback);
private:
    void _beginHeartbeatIfNoneActive();
    void _endActiveHeartbeat();

    IClock* _clockService = nullptr;
    IIntensitySensor* _intensitySensorService = nullptr;

    Heartbeat* _active_heartbeat = nullptr;
    uint16_t _active_heartbeat_timestamp;

    Pulse _previous_pulse = {0, 0};
    Pulse _last_recorded_pulse = {0, 0};

    std::function<void(Heartbeat*)> _callback;
};

#endif
