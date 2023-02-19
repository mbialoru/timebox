#include "defines.hpp"

/* Description
==============
boost::process as an alternative to less secure popen
==============
*/

#if PROTO_BOOSTPROCESS

#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <future>
#include <gtest/gtest.h>
#include <string>

TEST(Proto_boostprocess, capture_output)
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

#endif