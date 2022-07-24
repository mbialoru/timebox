#include <gtest/gtest.h>

#include "serialcontroller.hpp"
#include "clockcontroller.hpp"
#include "pid.hpp"


class Test_Combined : public ::testing::Test
{
protected:
  SerialController sc{ "/dev/ttyACM0", 9600, std::bind(&Test_Combined::Trigger, this) };
  ClockController cc{ 0, 0.001 };

public:
  void SetUp() override {};
  void TearDown() override {};
  void Trigger();
};

void Test_Combined::Trigger()
{
  cc.AdjustClock(sc.getBufferString());
}

TEST_F(Test_Combined, try_to_adjust_clock_mockup)
{
  sleep(20);
}