/* Description
==============
boost::process as an alternative to less secure popen
==============
*/

#include <boost/process.hpp>
#include <boost/process/async.hpp>
#include <future>
#include <gtest/gtest.h>
#include <string>

TEST(Proto_boostprocess, capture_output)
{
  // NOTE: First launch cmd/powershell and pass command as argument to it
  boost::asio::io_context ios;
  std::future<std::string> output;
  boost::process::system("cmd /c dir", boost::process::std_out > output, ios);
  auto res = output.get();
  std::cout << res;
}