#include <gtest/gtest.h>

#include "defines.hpp"
#include "pid.hpp"


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
  pid.SetLimits(10, 30);

  for (std::size_t i = 0; i < 50; i++) {
    if (i >= 9) {
      pid.SetTarget(100);
      pid.UpdateRaw(this->temperature(), 1);
    }
    if (pid.GetTarget() > 0) {
      auto pid_output = pid.GetOutputRaw();
      EXPECT_TRUE((pid_output >= 10 && pid_output <= 30));
    }
  }
}

TEST_F(Test_PID, control_temperature)
{
  for (std::size_t i = 0; i < 50; i++) {
    if (i >= 9) {
      pid.SetTarget(100);
      pid.UpdateRaw(this->temperature(), 1);
    }
    if (pid.GetTarget() > 0) this->temperature(pid.GetOutputRaw() - 1 / i);
  }
  EXPECT_NEAR(this->temperature(), 100, 0.01);
}