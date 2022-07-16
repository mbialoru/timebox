#include <gtest/gtest.h>
#include "utils.hpp"


TEST(test_utils, timesync_service_running)
{
  if (RunningFromDockerContainer())
    EXPECT_EQ(TimeSyncServiceIsRunning(), false);
  else
    EXPECT_EQ(TimeSyncServiceIsRunning(), true);
}