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
    static double _temperature{ 15 };
    _temperature += t / 4;
    return _temperature;
  }

public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(Test_PID, Control_Temperature)
{
  for (std::size_t i = 0; i < 50; i++) {
    if (i >= 9) {
      pid.SetTarget(60);
      pid.Update(this->temperature(), 1);
    }
    if (pid.GetTarget() > 0) this->temperature(pid.GetOutput() - 1 / i);
  }
  EXPECT_NEAR(this->temperature(), 60, 0.01);
}