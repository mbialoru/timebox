#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

#if USING_REAL_HARDWARE
#include "serialinterface.hpp"
#else
#include "mock.hpp"
#endif

using namespace TimeBox;

int MyCallback_calls{ 0 };
void MyCallback(TimeboxReadout) { MyCallback_calls++; }

class Test_SerialInterface : public ::testing::Test
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

TEST_F(Test_SerialInterface, thread_callback)
{
#if USING_REAL_HARDWARE
  SerialInterface sr{ std::bind(MyCallback, std::placeholders::_1) };
  sr.Open("COM3", 9600);
  sr.FlushIOBuffers();
#else
  MockSerialReader sr{ std::bind(&Test_SerialInterface::CallbackDummy, this) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(callback_calls > 3);
#if USING_REAL_HARDWARE
  sr.FlushIOBuffers();
  sr.Close();
#endif
}

TEST_F(Test_SerialInterface, using_free_function_callback)
{
#if USING_REAL_HARDWARE
  SerialInterface sr{ std::bind(MyCallback, std::placeholders::_1) };
  sr.Open("COM3", 9600);
  sr.FlushIOBuffers();
#else
  MockSerialReader sr{ std::bind(MyCallback, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(MyCallback_calls > 3);
#if USING_REAL_HARDWARE
  sr.FlushIOBuffers();
  sr.Close();
#endif
}