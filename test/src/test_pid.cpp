#include <gtest/gtest.h>
#include "pid.hpp"


class PIDTest : public ::testing::Test
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

TEST_F(PIDTest, Control_Temperature)
{
  for (std::size_t i = 0; i < 50; i++)
  {
    if (i >= 9)
    {
      pid.set_target(60);
      pid.update(this->temperature(), 1);
    }
    if (pid.get_target() > 0)
      this->temperature(pid.get_output() - 1 / i);
  }
  EXPECT_NEAR(this->temperature(), 60, 0.01);
}