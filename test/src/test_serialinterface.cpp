#include <gtest/gtest.h>
#include "serialcontroller.hpp"

class SerialControllerTest : public ::testing::Test
{
protected:
  int callback_calls{ 0 };

public:
  void SetUp() override {};
  void TearDown() override {};

  void CallbackDummy()
  {
    // This cannot be a member function for some reason, cannot access this method
    // from within test body
    std::cout << " Callback Launched " << callback_calls << std::endl;
    callback_calls++;
  }
};

TEST_F(SerialControllerTest, thread_callback)
{
  // GTEST_SKIP() << "Long running";
  SerialController si{ "/dev/ttyACM0", 9600, std::bind(&SerialControllerTest::CallbackDummy, this) };

  // When warm starting, arduino needs about ~5s to initizize and start sending
  // When cold starting GPS module will need much longer to obtain signal !
  std::this_thread::sleep_for(std::chrono::seconds(30));
  EXPECT_TRUE(callback_calls > 5);
}

void MyCallback()
{
  std::cout << " Free function callback Launched ";
}

TEST_F(SerialControllerTest, using_free_function_callback)
{
  GTEST_SKIP();
  SerialController si{ "/dev/ttyACM0", 9600, std::function(MyCallback) };
}