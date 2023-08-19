#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, timesync_service_running)
{
  if (check_if_using_docker())
    EXPECT_EQ(check_ntp_status(), false);
  else
    EXPECT_EQ(check_ntp_status(), true);
}

TEST(Test_Utilities, serial_devices_list)
{
  if (check_if_using_docker()) { GTEST_SKIP() << "Cannot run from Docker container !"; }
  GTEST_SKIP() << "Test not implemented";
}