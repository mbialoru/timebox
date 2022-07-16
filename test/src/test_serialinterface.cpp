#include <gtest/gtest.h>
#include <functional>
#include "serialinterface.hpp"

int callback_calls{ 0 };

void CallbackDummy()
{
  // This cannot be a member function for some reason, cannot access this method
  // from within test body
  std::cout << " Callback Launched " << callback_calls << std::endl;
  callback_calls++;
}

TEST(SerialInterfaceTest, creating_objects)
{
  GTEST_SKIP() << "Test ends too quickly";
  SerialInterface si{ "/dev/ttyACM0", 9600, &CallbackDummy };
}

TEST(SerialInterfaceTest, thread_callback)
{
  SerialInterface si{ "/dev/ttyACM0", 9600, &CallbackDummy };
  // When warm starting, arduino needs about ~5s to initizize and start sending
  // When cold starting GPS module will need much longer to obtain signal !
  sleep(10);
  EXPECT_TRUE(callback_calls > 0);
}