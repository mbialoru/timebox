#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

#if USING_REAL_HARDWARE
#if defined(__unix__)
#include "linserialreader.hpp"

#elif defined(_WIN64) && !defined(__CYGWIN__)
#include "winserialreader.hpp"
#endif
#else
#include "fakes.hpp"
#endif

using namespace TimeBox;

class Test_SerialReader : public ::testing::Test
{
protected:
  int callback_calls{ 0 };

public:
  void SetUp() override{};
  void TearDown() override{};

  void CallbackDummy() { callback_calls++; }
};

int MyCallback_calls{ 0 };
void MyCallback(TimeboxReadout) { MyCallback_calls++; }

TEST_F(Test_SerialReader, thread_callback)
{
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS;

#if USING_REAL_HARDWARE
#if defined(__unix__)
  LinSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(MyCallback, std::placeholders::_1) };
#elif defined(_WIN64) && !defined(__CYGWIN__)
  WinSerialReader sr{ "COM1", 9600, std::bind(MyCallback, std::placeholders::_1) };
#endif
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&Test_SerialReader::CallbackDummy, this) };
#endif

  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(callback_calls > 3);
}

TEST_F(Test_SerialReader, using_free_function_callback)
{
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS = " << LONG_TESTS;

#if USING_REAL_HARDWARE
#if defined(__unix__)
  LinSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(MyCallback, std::placeholders::_1) };
#elif defined(_WIN64) && !defined(__CYGWIN__)
  WinSerialReader sr{ "COM1", 9600, std::bind(MyCallback, std::placeholders::_1) };
#endif
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(MyCallback, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(MyCallback_calls > 3);
}