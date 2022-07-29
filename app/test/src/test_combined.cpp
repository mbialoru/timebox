#include <gtest/gtest.h>

#include "defines.hpp"
#include "clockcontroller.hpp"
#include "serialreader.hpp"


class Test_Combined : public ::testing::Test
{
public:
  void SetUp() override {};
  void TearDown() override {};
};

TEST_F(Test_Combined, try_to_adjust_clock_mockup)
{
  GTEST_SKIP();
  ClockController cc{ 0, 0.001 };
  SerialReader sc{ "/dev/ttyACM0", 9600, std::bind(&ClockController::AdjustClock, &cc, std::placeholders::_1) };

  std::this_thread::sleep_for(std::chrono::seconds(10));
}