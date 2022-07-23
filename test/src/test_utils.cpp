#include <gtest/gtest.h>
#include "utils.hpp"


TEST(Test_utils, timesync_service_running)
{
  if (RunningFromDockerContainer())
    EXPECT_EQ(TimeSyncServiceIsRunning(), false);
  else
    EXPECT_EQ(TimeSyncServiceIsRunning(), true);
}

TEST(Test_utils, timepoint_from_string)
{
  std::string test_str{ "9:31:10.0" };
  auto res = TimepointFromString(test_str);
  auto diff = res - std::chrono::system_clock::now();
  EXPECT_TRUE(diff.count() < 0);
}