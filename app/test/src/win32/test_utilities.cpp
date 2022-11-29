#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, timesync_service_running)
{
  // TODO: This could be conditional and detect both cases
  EXPECT_EQ(CheckNTPService(), true);
}

TEST(Test_Utilities, convert_baud_rate) { EXPECT_EQ(ConvertBaudRate(9600), CBR_9600); }

TEST(Test_Utilities, serial_devices_list)
{
  EXPECT_NO_THROW({
    for (const auto &val : GetSerialDevicesList()) { BOOST_LOG_TRIVIAL(debug) << val; }
  });
}