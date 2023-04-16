#include <Arduino.h>
#include <unity.h>

// #include "Heartbeat.h"

using namespace fakeit;

#include "HeartbeatReader.h"

void setUp(void)
{
    ArduinoFakeReset();
}

void test_heartbeat_empty_after_initialization() {
    Heartbeat heartbeat;

    TEST_ASSERT_TRUE(heartbeat.empty());
}

void test_heartbeat_is_not_empty_after_recording_a_pulse() {
    Heartbeat heartbeat;
    Pulse p1 = {0, 500};
    heartbeat.recordPulse(p1);

    TEST_ASSERT_FALSE(heartbeat.empty());
}

void test_heartbeat_pulses_returns_a_vector_iterator_with_all_the_recorded_pulses() {
    Heartbeat heartbeat;
    Pulse p1 = {0, 500};
    Pulse p2 = {255, 1000};
    heartbeat.recordPulse(p1);
    heartbeat.recordPulse(p2);

    std::vector<Pulse> pulses = heartbeat.pulses();
    std::vector<Pulse>::iterator pulses_it = pulses.begin();

    TEST_ASSERT_TRUE(*pulses_it == p1);
    pulses_it++;
    TEST_ASSERT_TRUE(*pulses_it == p2);
}

void test_heartbeat_reader_generates_a_heartbeat_when_sensor_is_idle_for_4000_ms() {
    Mock<IClock> clockMock;
    Mock<IIntensitySensor> intensitySensorMock;
    IClock& clock = clockMock.get();
    IIntensitySensor& instensitySensor = intensitySensorMock.get();
    HeartbeatReader heartbeatReader(&clock, &instensitySensor);
    std::vector<Pulse> received_pulses = {
        {1, 100},
        {2, 200},
        {3, 300},
        {4, 4301}
    };

    unsigned int callback_called = 0;
    heartbeatReader.setNewHeartbeatCallback([&callback_called](Heartbeat* hb) {
        std::vector<Pulse> pulses = hb->pulses();
        std::vector<Pulse>::iterator pulses_it = pulses.begin();

        TEST_ASSERT_EQUAL_UINT(100, pulses_it->timestamps_millis);
        TEST_ASSERT_EQUAL_UINT16(1, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(200, pulses_it->timestamps_millis);
        TEST_ASSERT_EQUAL_UINT16(2, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(300, pulses_it->timestamps_millis);
        TEST_ASSERT_EQUAL_UINT16(3, pulses_it->intensity);
        pulses_it++;

        TEST_ASSERT_TRUE(pulses_it == pulses.end());
        
        callback_called++;
    });

    for(Pulse received_pulse : received_pulses) {
        When(Method(clockMock, millis)).Return(received_pulse.timestamps_millis);
        When(Method(intensitySensorMock, read)).Return(received_pulse.intensity);
        heartbeatReader.loop();
    }

    TEST_ASSERT_EQUAL_UINT(1, callback_called);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_heartbeat_empty_after_initialization);
    RUN_TEST(test_heartbeat_is_not_empty_after_recording_a_pulse);
    
    RUN_TEST(test_heartbeat_reader_generates_a_heartbeat_when_sensor_is_idle_for_4000_ms);
    
    return UNITY_END();
}
