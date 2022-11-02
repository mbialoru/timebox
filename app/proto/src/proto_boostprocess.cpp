#include "defines.hpp"

/* Description
==============
boost::process as an alternative to less secure popen
==============
*/
#ifdef PROTO_BOOSTPROCESS

#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <future>
#include <gtest/gtest.h>
#include <string>

TEST(Proto_boostprocess, example_name)
{
  boost::asio::io_context ios;
  std::future<std::string> output;
  boost::process::system("cmd /c dir", boost::process::std_out > output, ios);
  auto res = output.get();
  std::cout << res;
}

#endif