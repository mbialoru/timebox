#include "defines.hpp"

/* Description
==============
Prototype to retrieve stream of serial data from arduino
==============
*/

#if PROTO_ARDUINOREAD

#include <gtest/gtest.h>
#include <fstream>
#include <cstdlib>

class ArduinoReadTest : public ::testing::Test
{
protected:
  const char* dev = "/dev/ttyACM0";

public:
  void SetUp() override {}
  void TearDown() override {}
};

TEST_F(ArduinoReadTest, read_from_ttyACM0)
{
  // Activate a tty connection
  system("stty -F /dev/ttyACM0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost \
  -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke \
  noflsh -ixon -crtscts");

  std::ifstream arduino_in(dev);
  std::unique_ptr<time_t> p_time{ std::make_unique<time_t>() };
  std::time(p_time.get());

  std::string time_str;

  for (std::size_t i = 0; i < 100;)
  {
    std::time(p_time.get());

    while (!arduino_in.eof())
    {
      arduino_in >> time_str;
      i++;
    }
    arduino_in.clear();
  }
  arduino_in.close();
}

#endif