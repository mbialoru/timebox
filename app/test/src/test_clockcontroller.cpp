#include <gtest/gtest.h>

#include "clockcontroller.hpp"
#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

class Test_ClockController : public ::testing::Test
{
public:
  void SetUp() override{};
  void TearDown() override{};
};

TEST_F(Test_ClockController, adjust_clock)
{
  if (CheckIfUsingDocker()) { GTEST_SKIP() << "Cannot run from Docker container !"; }

  std::shared_ptr<PID<double>> p_pid{ std::make_shared<PID<double>>(2.0, 1.0, 0.001, 0) };
  std::unique_ptr<ClockController> cc{ std::make_unique<ClockController>(0, 0.001, p_pid) };
  TimeboxReadout readout{ ConvertTimepointToString(std::chrono::system_clock::now() - std::chrono::seconds(10)) + ".0",
    std::chrono::system_clock::now() };
  std::this_thread::sleep_for(std::chrono::milliseconds(600));

  if (not CheckAdminPrivileges())
    EXPECT_THROW(cc->AdjustClock(readout), InsufficientPermissionsError) << "Not running as root !";
  else
    EXPECT_NO_THROW(cc->AdjustClock(readout));
}
