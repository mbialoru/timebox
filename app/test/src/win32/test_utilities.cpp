#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, timesync_service_running) { EXPECT_EQ(CheckNTPService(), true); }

TEST(Test_Utilities, serial_devices_list)
{
  EXPECT_NO_THROW({
    for (const auto &val : GetSerialDevicesList()) { BOOST_LOG_TRIVIAL(debug) << val; }
  });
}