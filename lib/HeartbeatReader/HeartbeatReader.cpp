#include "HeartbeatReader.h"


HeartbeatReader::HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService) {
    _clockService = clockService;
    _intensitySensorService = intensitySensorService;
}

void HeartbeatReader::loop() {
    if (_current_heartbeat == nullptr) {
        _current_heartbeat = new Heartbeat();
    }

    unsigned int pulse_timestamp_millis = _clockService->millis();
    uint16_t sensor_read_value =  _intensitySensorService->read();

    Pulse next_pulse = {
        sensor_read_value,
        pulse_timestamp_millis
    };

    if (pulse_timestamp_millis - _last_timestamp > 4000) {
        _callback(_current_heartbeat);
        _current_heartbeat = nullptr;
        return;
    }

    _current_heartbeat->recordPulse(next_pulse);
    _last_timestamp = pulse_timestamp_millis;
};

void HeartbeatReader::setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback) {
    _callback = callback;
};