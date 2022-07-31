#include <gtest/gtest.h>

#include "clockcontroller.hpp"
#include "defines.hpp"

#if USING_REAL_HARDWARE
#include "serialreader.hpp"
#else
#include "fakes.hpp"
#endif


class Test_Combined : public ::testing::Test
{
public:
  void SetUp() override{};
  void TearDown() override{};
};

TEST_F(Test_Combined, try_to_adjust_clock_mockup)
{
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS " << LONG_TESTS;

  ClockController cc{ 0, 0.001 };
#if USING_REAL_HARDWARE
  SerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&ClockController::AdjustClock, &cc, std::placeholders::_1) };
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&ClockController::AdjustClock, &cc, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(10));
}