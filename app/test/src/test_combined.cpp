#include <gtest/gtest.h>

#include "defines.hpp"

#if USING_REAL_HARDWARE
#if defined(__unix__)
#include "serialreader.hpp"

#elif defined(_WIN64) && !defined(__CYGWIN__)
#endif

#else
#if defined(__unix__)
#include "linclockcontroller.hpp"
#include "pid.hpp"

#elif defined(_WIN64) && !defined(__CYGWIN__)
#endif

#include "fakes.hpp"
#endif

using namespace TimeBox;

class Test_Combined : public ::testing::Test
{
public:
  void SetUp() override{};
  void TearDown() override{};
};

#if defined(__unix__)

TEST_F(Test_Combined, linux_combined_test)
{
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS;

  std::shared_ptr<PID<double>> p_pid{ std::make_shared<PID<double>>(2.0, 1.0, 0.001, 0) };
  LinClockController cc{ 0, p_pid, 0.001 };
#if USING_REAL_HARDWARE
  SerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&ClockController::AdjustClock, &cc, std::placeholders::_1) };
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&LinClockController::AdjustClock, &cc, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(10));
}

#elif defined(_WIN64) && !defined(__CYGWIN__)

TEST_F(Test_Combined, windows_combined_test) {}

#endif