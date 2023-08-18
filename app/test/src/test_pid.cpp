#include <gtest/gtest.h>

#include "pid.hpp"

using namespace TimeBox;

class Test_PID : public ::testing::Test
{
protected:
  PID<double> pid{ 1.2, 1.0, 0.001, 0 };

  double temperature(double t = 0.0)
  {
    // Could be its own class, but this is more compact
    static double temperature{ 15 };
    temperature += t / 4;
    return temperature;
  }

public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(Test_PID, limited_output)
{
  pid.set_limits(10, 30);

  for (std::size_t i = 0; i < 50; ++i) {
    if (i >= 9) {
      pid.set_target(100);
      pid.update_limited(temperature(), 1);
    }
    if (pid.get_target() > 0) {
      auto pid_output = pid.get_output_limited();
      EXPECT_TRUE((pid_output >= 10 && pid_output <= 30));
    }
  }
}

TEST_F(Test_PID, control_temperature)
{
  for (std::size_t i = 0; i < 50; ++i) {
    if (i >= 9) {
      pid.set_target(100);
      pid.update_raw(temperature(), 1);
    }
    if (pid.get_target() > 0) temperature(pid.get_output_raw() - 1 / i);
  }
  EXPECT_NEAR(temperature(), 100, 0.01);
}