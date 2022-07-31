#include <gtest/gtest.h>

#include "defines.hpp"

#if USING_REAL_HARDWARE
#include "serialreader.hpp"
#else
#include "fakes.hpp"
#endif


class Test_SerialReader : public ::testing::Test
{
protected:
  int callback_calls{ 0 };

public:
  void SetUp() override{};
  void TearDown() override{};

  void CallbackDummy() { callback_calls++; }
};

TEST_F(Test_SerialReader, thread_callback)
{
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS " << LONG_TESTS;

#if USING_REAL_HARDWARE
  SerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&Test_SerialReader::CallbackDummy, this) };
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(&Test_SerialReader::CallbackDummy, this) };
#endif

  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(callback_calls > 3);
}

int MyCallback_calls{ 0 };
void MyCallback(std::string) { MyCallback_calls++; }

TEST_F(Test_SerialReader, using_free_function_callback)
{
  if (!LONG_TESTS) GTEST_SKIP() << "Skipping, LONG_TESTS " << LONG_TESTS;

#if USING_REAL_HARDWARE
  SerialReader sr{ "/dev/ttyACM0", 9600, std::bind(MyCallback, std::placeholders::_1) };
#else
  FakeSerialReader sr{ "/dev/ttyACM0", 9600, std::bind(MyCallback, std::placeholders::_1) };
#endif
  std::this_thread::sleep_for(std::chrono::seconds(5));
  EXPECT_TRUE(MyCallback_calls > 3);
}