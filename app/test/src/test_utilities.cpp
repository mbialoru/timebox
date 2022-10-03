#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

// int DummyFunctionA()
// {
//   usleep(20000);
//   return 5;
// }

// int DummyFunctionB(int a)
// {
//   usleep(20000);
//   return a + 5;
// }

// void DummyFunctionC() { usleep(20000); }

TEST(Test_Utilities, timesync_service_running)
{
#if defined(__unix__)
  if (CheckIfUsingDocker())
    EXPECT_EQ(CheckNTPService(), false);
  else
    EXPECT_EQ(CheckNTPService(), true);
#elif defined(_WIN64) && !defined(__CYGWIN__)
  // TODO: This could be conditional and detect both cases
  EXPECT_EQ(CheckNTPService(), true);
#endif
}

TEST(Test_Utilities, timepoint_from_string)
{
  std::string test_str{ "7:31:10.0" };
  auto res = ConvertStringToTimepoint(test_str);
  auto diff = res - std::chrono::system_clock::now();
  EXPECT_TRUE(diff.count() < 0);
}

TEST(Test_Utilities, string_from_timepoint)
{
  std::string test_str{ "12:34:56.7" };
  auto res = ConvertStringToTimepoint(test_str);
  auto res_str = ConvertTimepointToString(res);
  EXPECT_EQ(res_str, "12:34:56");
}

TEST(Test_Utilities, convert_baud_rate)
{
#if defined(__unix__)
  EXPECT_EQ(ConvertBaudRate(9600), B9600);
#elif defined(_WIN64) && !defined(__CYGWIN__)
  EXPECT_EQ(ConvertBaudRate(9600), CBR_9600);
#endif
}

// TEST(Test_Utilities, serial_devices_list)
// {
//   if (CheckIfUsingDocker()) { GTEST_SKIP() << "Cannot run from Docker container !"; }

//   std::vector<std::string> tmp;
//   tmp = GetSerialDevicesList();
//   for (const auto &val : tmp) { BOOST_LOG_TRIVIAL(debug) << val; }
// }

// TEST(Test_Utilities, timing_decorator)
// {
//   auto decorated_dummy_a = WrapTimingDecorator(DummyFunctionA);
//   auto [value, run_time] = decorated_dummy_a();
//   EXPECT_EQ(value, 5);
//   EXPECT_NEAR(run_time, 20, 1);
// }

// TEST(Test_Utilities, timing_decorator_arguments)
// {
//   auto decorated_dummy_b = WrapTimingDecorator(DummyFunctionB);
//   auto [value, run_time] = decorated_dummy_b(5);
//   EXPECT_EQ(value, 10);
// }

// TEST(Test_Utilities, timing_decorator_void_type)
// {
//   auto decorated_dummy_c = WrapTimingDecorator(DummyFunctionC);
//   auto run_time = decorated_dummy_c();
//   EXPECT_NEAR(run_time, 20, 1);
// }