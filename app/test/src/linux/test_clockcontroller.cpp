#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "clockcontroller.hpp"
#include "defines.hpp"
#include "mock.hpp"
#include "pid.hpp"
#include "utilities.hpp"


using namespace TimeBox;

class Test_ClockController : public ::testing::Test
{
public:
  void SetUp() override
  {
    if (check_if_using_docker()) { GTEST_SKIP() << "Cannot run from Docker container"; }
    if (not check_admin_privileges()) { GTEST_SKIP() << "Cannot run without admin privileges"; }
  };
};

TEST_F(Test_ClockController, adjust_clock)
{
  // Retrieve original tick value
  timex t{};
  auto res = adjtimex(&t);
  EXPECT_EQ(res, 0) << "PRE: Failed to retrieve timex";
  auto pre_tick = t.tick;

  std::shared_ptr<PID<double>> p_pid{ std::make_shared<PID<double>>(2.0, 1.0, 0.001, 0) };
  std::unique_ptr<ClockController> p_clockcontroller{ std::make_unique<LinClockController>(0, p_pid, 0.001) };

  TimeboxReadout readout{ timepoint_to_string(std::chrono::system_clock::now() - std::chrono::seconds(10)) + ".0",
    std::chrono::system_clock::now() };
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  EXPECT_NO_THROW(p_clockcontroller->adjust_clock(readout));

  t = timex();
  res = adjtimex(&t);
  EXPECT_EQ(res, 0) << "IN: Failed to retrieve timex";
  auto in_tick = t.tick;
  EXPECT_NE(in_tick, pre_tick) << "Failed to set tick to different value";

  // Desctructor of LinClockController restores tick value
  p_clockcontroller.reset();

  // Retrieve value after changes
  t = timex();
  res = adjtimex(&t);
  EXPECT_EQ(res, 0) << "POST: Failed to retrieve timex";
  auto post_tick = t.tick;
  EXPECT_EQ(pre_tick, post_tick) << "Failed to reset tick to original value";
}
