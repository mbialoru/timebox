#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

#if USING_REAL_HARDWARE
#include "serialreader.hpp"
#else
#include "fakes.hpp"
#endif

using namespace TimeBox;

class Test_SerialReader : public ::testing::Test
{
protected:
  int callback_calls{ 0 };

public:
  void SetUp() override
  {
    if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS;
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  };
  void TearDown() override
  {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  };

  void CallbackDummy() { callback_calls++; }
};

int MyCallback_calls{ 0 };
void MyCallback(TimeboxReadout) { MyCallback_calls++; }

TEST_F(Test_SerialReader, thread_callback)
{
#if USING_REAL_HARDWARE
  SerialReader sr{ std::bind(MyCallback, std::placeholders::_1) };
#else
  MockSerialReader sr{ std::bind(&Test_SerialReader::CallbackDummy, this) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(callback_calls > 3);
}

TEST_F(Test_SerialReader, using_free_function_callback)
{
#if USING_REAL_HARDWARE
  SerialReader sr{ std::bind(MyCallback, std::placeholders::_1) };
#else
  MockSerialReader sr{ std::bind(MyCallback, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(MyCallback_calls > 3);
}

TEST(Test_SerialReader, new_implementation) { GTEST_FAIL(); }