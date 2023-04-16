#include "HeartbeatReader.h"


HeartbeatReader::HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService) {
    _clockService = clockService;
    _intensitySensorService = intensitySensorService;
}

void HeartbeatReader::loop() {
    if (!_current_heartbeat) {
        _current_heartbeat = new Heartbeat();
    }

    unsigned int pulse_timestamp_millis = _clockService->millis();
    uint16_t sensor_read_value =  _intensitySensorService->read();

    Pulse next_pulse = {
        sensor_read_value,
        pulse_timestamp_millis
    };

    if (next_pulse.timestamps_millis - _last_pulse.timestamps_millis > 4000) {
        _callback(_current_heartbeat);
        _current_heartbeat = nullptr;
        return;
    }

    _current_heartbeat->recordPulse(next_pulse);
    _last_pulse = next_pulse;
};

void HeartbeatReader::setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback) {
    _callback = callback;
};