#include <gtest/gtest.h>

#include "defines.hpp"
#include "utils.hpp"
#include "clockcontroller.hpp"


class Test_ClockController : public ::testing::Test
{
public:
  void SetUp() override {};
  void TearDown() override {};
};

TEST_F(Test_ClockController, adjust_clock)
{
  std::unique_ptr<ClockController> cc{ std::make_unique<ClockController>(0, 0.001) };
  auto time = std::chrono::system_clock::now() - std::chrono::seconds(10);
  auto time_str = StringFromTimepoint(time);
  std::this_thread::sleep_for(std::chrono::milliseconds(600));
  if (not RunningAsRoot())
    EXPECT_THROW(cc->AdjustClock(time_str + ".0"),
      InsufficientPermissionsError) << "Not running as root !";
  else
    EXPECT_NO_THROW(cc->AdjustClock(time_str + ".0"));
}
