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

#if defined(__unix__)
TEST(Proto_boostprocess, example_name)
{
  // NOTE: Do not launch in docker container
  boost::asio::io_context ios;
  std::future<std::string> output;
  boost::process::system("pidof systemd-timesyncd", boost::process::std_out > output, ios);
  auto res = output.get();
  if (not res.empty()) {
    std::cout << std::stoi(res);
  } else {
    GTEST_FAIL();
  }
}
#elif defined(_WIN64) && !defined(__CYGWIN__)
TEST(Proto_boostprocess, example_name)
{
  boost::asio::io_context ios;
  std::future<std::string> output;
  boost::process::system("cmd /c dir", boost::process::std_out > output, ios);
  auto res = output.get();
  std::cout << res;
}
#endif

#endif