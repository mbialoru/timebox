#include <gtest/gtest.h>

#include "defines.hpp"

#if USING_REAL_HARDWARE
#if defined(__unix__)
#include "linclockcontroller.hpp"
#include "linserialreader.hpp"
#include "pid.hpp"

#elif defined(_WIN64) && !defined(__CYGWIN__)
#include "winclockconroller.hpp"
#include "winserialreader.hpp"
#endif

#else
#include "fakes.hpp"
#endif

using namespace TimeBox;

class Test_Combined : public ::testing::Test
{
public:
  void SetUp() override
  {
    if (not LONG_TESTS) { GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS; }
  };
  void TearDown() override{};
};

TEST_F(Test_Combined, fake_combined_test)
{
  FakeClockController cc;
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&FakeClockController::AdjustClock, &cc, std::placeholders::_1) };
  std::this_thread::sleep_for(std::chrono::seconds(10));
}

#if defined(__unix__)

TEST_F(Test_Combined, linux_combined_test)
{
  if (not CheckAdminPrivileges()) { GTEST_SKIP() << "Skipping, requires admin privileges"; }
}

#elif defined(_WIN64) && !defined(__CYGWIN__)

TEST_F(Test_Combined, windows_combined_test) {}

#endif