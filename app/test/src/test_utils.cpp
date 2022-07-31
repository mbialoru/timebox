#include <gtest/gtest.h>

#include "defines.hpp"
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

TEST(Test_utils, string_from_timepoint)
{
  std::string test_str{ "12:34:56.7" };
  auto res = TimepointFromString(test_str);
  auto res_str = StringFromTimepoint(res);
  EXPECT_EQ(res_str, "12:34:56");
}

TEST(Test_utils, convert_baud_rate) { EXPECT_EQ(ConvertBaudRate(9600), B9600); }