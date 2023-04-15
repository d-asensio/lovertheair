#include <unity.h>
#include "Heartbeat.h"

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

    std::vector<Pulse>::iterator pulses_it = heartbeat.pulses();

    TEST_ASSERT_TRUE(*pulses_it == p1);
    pulses_it++;
    TEST_ASSERT_TRUE(*pulses_it == p2);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_heartbeat_empty_after_initialization);
    RUN_TEST(test_heartbeat_is_not_empty_after_recording_a_pulse);
    
    UNITY_END();
}
