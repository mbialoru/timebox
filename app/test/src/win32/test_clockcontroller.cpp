// #include <boost/log/trivial.hpp>
// #include <gtest/gtest.h>

// #include "clockcontroller.hpp"
// #include "defines.hpp"
// #include "mock.hpp"
// #include "pid.hpp"
// #include "utilities.hpp"

// using namespace TimeBox;

// class Test_ClockController : public ::testing::Test
// {
// public:
//   void SetUp() override
//   {
//     if (not CheckAdminPrivileges()) { GTEST_SKIP() << "Cannot run without admin privileges !"; }
//   };
// };

// TEST_F(Test_ClockController, adjust_clock)
// {
//   DWORD current_adjustment_legacy{ 0 };
//   DWORD initial_adjustment_legacy{ 0 };
//   DWORD time_increment_legacy{ 0 };
//   BOOL enabled_legacy{ 0 };

//   if (not GetSystemTimeAdjustment(&initial_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
//     BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment" << HRESULT_FROM_WIN32(GetLastError());
//     GTEST_FAIL();
//   }

//   std::shared_ptr<PID<double>> p_pid{ std::make_shared<PID<double>>(2.0, 1.0, 0.001, 0) };
//   std::unique_ptr<ClockController> p_clockcontroller{ std::make_unique<ClockController>(0, p_pid, 0.001) };

//   p_pid->SetLimits(-1000, 1000);// Without limited PID, we get output of 0 from it

//   TimeboxReadout readout{ ConvertTimepointToString(std::chrono::system_clock::now() - std::chrono::seconds(10)) +
//   ".0",
//     std::chrono::system_clock::now() };
//   std::this_thread::sleep_for(std::chrono::milliseconds(500));
//   EXPECT_NO_THROW(p_clockcontroller->AdjustClock(readout));

//   if (not GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
//     BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment" << HRESULT_FROM_WIN32(GetLastError());
//     GTEST_FAIL();
//   }
//   EXPECT_LT(current_adjustment_legacy, initial_adjustment_legacy);

//   p_clockcontroller.reset();
//   if (not GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
//     BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment" << HRESULT_FROM_WIN32(GetLastError());
//     GTEST_FAIL();
//   }
//   EXPECT_EQ(current_adjustment_legacy, initial_adjustment_legacy);
// }
