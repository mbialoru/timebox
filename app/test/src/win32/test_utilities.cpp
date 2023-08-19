#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, timesync_service_running) { EXPECT_EQ(check_ntp_status(), true); }

TEST(Test_Utilities, serial_devices_list)
{
  EXPECT_NO_THROW({
    for (const auto &val : get_serial_devices_list()) { BOOST_LOG_TRIVIAL(debug) << val; }
  });
}