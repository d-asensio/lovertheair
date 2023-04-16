#include <Arduino.h>
#include <unity.h>

#include "Heartbeat.h"
#include "HeartbeatReader.h"

using namespace fakeit;

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

void test_heartbeat_pulses_returns_a_vector_with_all_the_recorded_pulses() {
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

void test_heartbeat_does_not_record_two_consecutive_equal_pulses() {
    Heartbeat heartbeat;
    Pulse p1 = {128, 500};
    Pulse p2 = {128, 500};
    heartbeat.recordPulse(p1);
    heartbeat.recordPulse(p2);

    std::vector<Pulse> pulses = heartbeat.pulses();
    std::vector<Pulse>::iterator pulses_it = pulses.begin();

    TEST_ASSERT_TRUE(*pulses_it == p1);
    pulses_it++;
    TEST_ASSERT_TRUE(pulses_it == pulses.end());
}

void test_heartbeat_reader_does_not_generate_any_heartbeat_if_no_pulses_are_received() {
    Mock<IClock> clockMock;
    Mock<IIntensitySensor> intensitySensorMock;
    IClock& clock = clockMock.get();
    IIntensitySensor& instensitySensor = intensitySensorMock.get();
    HeartbeatReader heartbeatReader(&clock, &instensitySensor);

    unsigned int callback_called = 0;
    heartbeatReader.setNewHeartbeatCallback([&callback_called](Heartbeat* hb) {        
        callback_called++;
    });

    TEST_ASSERT_EQUAL_UINT(0, callback_called);
}

void test_heartbeat_reader_generates_a_heartbeat_when_sensor_is_idle_for_more_than_4000_ms() {
    Mock<IClock> clockMock;
    Mock<IIntensitySensor> intensitySensorMock;
    IClock& clock = clockMock.get();
    IIntensitySensor& instensitySensor = intensitySensorMock.get();
    HeartbeatReader heartbeatReader(&clock, &instensitySensor);
    std::vector<Pulse> received_pulses = {
        {1, 100},
        {2, 200},
        {3, 300},
        {0, 4301}
    };

    unsigned int callback_called = 0;
    heartbeatReader.setNewHeartbeatCallback([&callback_called](Heartbeat* hb) {
        std::vector<Pulse> pulses = hb->pulses();
        std::vector<Pulse>::iterator pulses_it = pulses.begin();

        TEST_ASSERT_EQUAL_UINT(0, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(1, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(100, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(2, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(200, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(3, pulses_it->intensity);
        pulses_it++;

        TEST_ASSERT_TRUE(pulses_it == pulses.end());
        
        callback_called++;
    });

    for(Pulse received_pulse : received_pulses) {
        When(Method(clockMock, milliseconds)).AlwaysReturn(received_pulse.time);
        When(Method(intensitySensorMock, read)).Return(received_pulse.intensity);
        heartbeatReader.loop();
    }

    TEST_ASSERT_EQUAL_UINT(1, callback_called);
}

void test_heartbeat_reader_generates_a_heartbeat_when_sensor_reads_0_for_more_than_4000_ms() {
    Mock<IClock> clockMock;
    Mock<IIntensitySensor> intensitySensorMock;
    IClock& clock = clockMock.get();
    IIntensitySensor& instensitySensor = intensitySensorMock.get();
    HeartbeatReader heartbeatReader(&clock, &instensitySensor);
    std::vector<Pulse> received_pulses = {
        {1, 1000},
        {0, 2000},
        {3, 3000},
        {0, 4000},
        {0, 5000},
        {0, 6000},
        {0, 7000},
        {0, 8001}
    };

    unsigned int callback_called = 0;
    heartbeatReader.setNewHeartbeatCallback([&callback_called](Heartbeat* hb) {
        std::vector<Pulse> pulses = hb->pulses();
        std::vector<Pulse>::iterator pulses_it = pulses.begin();

        TEST_ASSERT_EQUAL_UINT(0, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(1, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(1000, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(0, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(2000, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(3, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(3000, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(0, pulses_it->intensity);
        pulses_it++;
        TEST_ASSERT_EQUAL_UINT(6000, pulses_it->time);
        TEST_ASSERT_EQUAL_UINT16(0, pulses_it->intensity);
        pulses_it++;

        TEST_ASSERT_TRUE(pulses_it == pulses.end());
        
        callback_called++;
    });

    for(Pulse received_pulse : received_pulses) {
        When(Method(clockMock, milliseconds)).AlwaysReturn(received_pulse.time);
        When(Method(intensitySensorMock, read)).Return(received_pulse.intensity);
        heartbeatReader.loop();
    }

    TEST_ASSERT_EQUAL_UINT(1, callback_called);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_heartbeat_empty_after_initialization);
    RUN_TEST(test_heartbeat_is_not_empty_after_recording_a_pulse);
    RUN_TEST(test_heartbeat_does_not_record_two_consecutive_equal_pulses);
    
    RUN_TEST(test_heartbeat_reader_does_not_generate_any_heartbeat_if_no_pulses_are_received);
    RUN_TEST(test_heartbeat_reader_generates_a_heartbeat_when_sensor_is_idle_for_more_than_4000_ms);
    RUN_TEST(test_heartbeat_reader_generates_a_heartbeat_when_sensor_reads_0_for_more_than_4000_ms);
    
    return UNITY_END();
}
