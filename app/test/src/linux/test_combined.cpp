#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"

#if USING_REAL_HARDWARE
#include "clockcontroller.hpp"
#include "serialreader.hpp"
#else
#include "mock.hpp"
#endif

using namespace TimeBox;

class Test_Combined : public ::testing::Test
{
public:
  void SetUp() override
  {
    if (not LONG_TESTS) { GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS; }
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  };
  void TearDown() override
  {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  };
};

#if USING_REAL_HARDWARE
TEST_F(Test_Combined, combined_test)
{
  if (not check_admin_privileges()) { GTEST_SKIP() << "Skipping, requires admin privileges"; }
  GTEST_SKIP() << "Test not implemented";
}
#else
TEST_F(Test_Combined, fake_combined_test)
{
  MockClockController cc;
  MockSerialReader sr{ std::bind(&MockClockController::adjust_clock, &cc, std::placeholders::_1) };
  std::this_thread::sleep_for(std::chrono::seconds(5));
}
#endif
