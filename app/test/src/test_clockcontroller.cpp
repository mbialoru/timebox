#include <gtest/gtest.h>

#include "defines.hpp"
#include "utils.hpp"
#include "clockcontroller.hpp"


class Test_ClockController : public ::testing::Test
{
protected:
  ClockController cc{ 0, 0.001 };

public:
  void SetUp() override {};
  void TearDown() override {};
};

TEST_F(Test_ClockController, adjust_clock)
{
  auto time = std::chrono::system_clock::now() - std::chrono::seconds(10);
  auto time_str = StringFromTimepoint(time);
  cc.AdjustClock(time_str + ".0");
}
