#include <stdint.h>

#include "HeartbeatReader.h"

HeartbeatReader::HeartbeatReader(IClock *clockService, IIntensitySensor *intensitySensorService)
{
    _clockService = clockService;
    _intensitySensorService = intensitySensorService;
}

// TODO
// - Previous pulse has to be erased when a new heartbeat is generated
void HeartbeatReader::loop()
{
    _beginHeartbeatIfNoneActive();

    unsigned long pulse_timestamp_millis = _clockService->milliseconds();
    uint16_t sensor_read_value = _intensitySensorService->read();

    Pulse next_pulse = {
        sensor_read_value,
        pulse_timestamp_millis - _active_heartbeat_timestamp
    };

    if (next_pulse.isIdle() && next_pulse.time - _last_recorded_pulse.time > 4000)
    {
        _active_heartbeat->recordPulse(_previous_pulse);
        _callback(_active_heartbeat);

        _endActiveHeartbeat();
        return;
    }

    if (next_pulse.intensity != _previous_pulse.intensity)
    {
        _active_heartbeat->recordPulse(next_pulse);
        // TODO Create a last pulse method in Heartbeat
        _last_recorded_pulse = next_pulse;
    }

    _previous_pulse = next_pulse;
};

void HeartbeatReader::setNewHeartbeatCallback(std::function<void(Heartbeat *)> callback)
{
    _callback = callback;
};

void HeartbeatReader::_beginHeartbeatIfNoneActive()
{
    if (!_active_heartbeat)
    {
        _active_heartbeat = new Heartbeat();
        _active_heartbeat_timestamp = _clockService->milliseconds();
    }
}

void HeartbeatReader::_endActiveHeartbeat()
{
    delete _active_heartbeat;
    _active_heartbeat = nullptr;
}