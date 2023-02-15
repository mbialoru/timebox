#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, timesync_service_running)
{
  if (CheckIfUsingDocker())
    EXPECT_EQ(CheckNTPService(), false);
  else
    EXPECT_EQ(CheckNTPService(), true);
}

TEST(Test_Utilities, serial_devices_list)
{
  if (CheckIfUsingDocker()) { GTEST_SKIP() << "Cannot run from Docker container !"; }
  GTEST_SKIP() << "Test not implemented";
}