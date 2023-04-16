#include "HeartbeatReader.h"


HeartbeatReader::HeartbeatReader(IClock* clockService, IIntensitySensor* intensitySensorService) {
    _clockService = clockService;
    _intensitySensorService = intensitySensorService;
}

void HeartbeatReader::loop() {
    if (!_current_heartbeat) {
        _current_heartbeat = new Heartbeat();
    }

    unsigned long pulse_timestamp_millis = _clockService->milliseconds();
    uint16_t sensor_read_value =  _intensitySensorService->read();

    Pulse next_pulse = {
        sensor_read_value,
        pulse_timestamp_millis
    };

    if (next_pulse.intensity == 0 && next_pulse.timestamps_millis - _last_recorded_pulse.timestamps_millis > 4000) {
        // TODO Implement this logic inside heatbeat
        if(_previous_pulse != _last_recorded_pulse) {
            _current_heartbeat->recordPulse(_previous_pulse);
        }
        _callback(_current_heartbeat);
        _current_heartbeat = nullptr;
        return;
    }

    if (next_pulse.intensity != _previous_pulse.intensity) {
        _current_heartbeat->recordPulse(next_pulse);
        // TODO Create a last pulse method in Heartbeat
        _last_recorded_pulse = next_pulse;
    }

    _previous_pulse = next_pulse;
};

void HeartbeatReader::setNewHeartbeatCallback(std::function<void(Heartbeat*)> callback) {
    _callback = callback;
};