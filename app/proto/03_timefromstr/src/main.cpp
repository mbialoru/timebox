/* Description
==============
Prototype to develop a way for obtaining time-like type from string acquired by
serial connection
==============
*/

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>

class Proto_TimeFromStr : public ::testing::Test
{
protected:
  std::vector<int> int_vec;
  std::string str{ "17:41:12.99" };
  std::vector<std::string> str_vec;

public:
};

const std::string currentDateTime()
{
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

  return buf;
}

TEST_F(Proto_TimeFromStr, disect_string_to_ints)
{
  GTEST_SKIP();
  boost::split(str_vec, str, boost::is_any_of(":."));

  for (auto &piece : str_vec) int_vec.push_back(std::stoi(piece));

  EXPECT_EQ("17", str_vec.at(0));
  EXPECT_EQ("41", str_vec.at(1));
  EXPECT_EQ("12", str_vec.at(2));
  EXPECT_EQ("99", str_vec.at(3));
  EXPECT_EQ(4, int_vec.size());
}

TEST_F(Proto_TimeFromStr, construct_time_type)
{
  GTEST_SKIP();
  boost::split(str_vec, str, boost::is_any_of(":."));

  for (auto &piece : str_vec) int_vec.push_back(std::stoi(piece));

  std::tm tm{};
  std::istringstream ss(str_vec.at(0) + ":" + str_vec.at(1) + ":" + str_vec.at(2) + ":" + str_vec.at(3));
  ss >> std::get_time(&tm, "%H:%M:%S");
  std::time_t time = mktime(&tm);

  auto from = std::chrono::system_clock::from_time_t(time);
  auto after = std::chrono::system_clock::to_time_t(from);

  BOOST_LOG_TRIVIAL(trace) << ctime(&after);
  BOOST_LOG_TRIVIAL(trace) << currentDateTime();
}

TEST_F(Proto_TimeFromStr, now_to_tm_struct)
{
  GTEST_SKIP();
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto tm = *std::localtime(&now);
  auto tm_time = std::mktime(&tm);

  BOOST_LOG_TRIVIAL(trace) << std::put_time(std::localtime(&tm_time), "%c %Z");
}

TEST_F(Proto_TimeFromStr, now_to_tm_struct_and_change_time)
{
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto tm = *std::localtime(&now);

  boost::split(str_vec, str, boost::is_any_of(":."));
  tm.tm_hour = std::stoi(str_vec[0]);
  tm.tm_min = std::stoi(str_vec[1]);
  tm.tm_sec = std::stoi(str_vec[2]);

  if (std::round(std::stof(str_vec[3]) / 100) == 1) tm.tm_sec++;

  auto tm_time = std::mktime(&tm);

  BOOST_LOG_TRIVIAL(trace) << std::put_time(std::localtime(&tm_time), "%c %Z");

  EXPECT_EQ(tm.tm_hour, 17);
  EXPECT_EQ(tm.tm_min, 41);
  EXPECT_EQ(tm.tm_sec, 13);
}

TEST_F(Proto_TimeFromStr, change_time_in_time_point_from_now)
{
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm = *std::localtime(&now);

  boost::split(str_vec, str, boost::is_any_of(":."));

  tm.tm_hour = std::stoi(str_vec[0]);
  tm.tm_min = std::stoi(str_vec[1]);
  tm.tm_sec = std::stoi(str_vec[2]);

  auto before = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  auto after = before + std::chrono::duration<int, std::milli>(std::stoi(str_vec[3]) * 10);

  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
  EXPECT_EQ(diff.count(), 990);

  // If we cast again to time_t and std::tm, we loose everything with higher
  // resolution than 1s - as expected.
  auto after_time = std::chrono::system_clock::to_time_t(after);
  auto test = std::put_time(std::localtime(&after_time), "%c %Z");

  BOOST_LOG_TRIVIAL(trace) << std::put_time(std::localtime(&after_time), "%c %Z");
}