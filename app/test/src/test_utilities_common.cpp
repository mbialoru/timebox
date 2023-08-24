#include <gtest/gtest.h>

#include "defines.hpp"
#include "utilities.hpp"

using namespace TimeBox;

TEST(Test_Utilities, string_to_timepoint_working)
{
  std::string test_str_a{ "7:30:10.0" };
  std::string test_str_b{ "9:30:10.0" };
  auto res_a = string_to_timepoint(test_str_a);
  auto res_b = string_to_timepoint(test_str_b);
  auto diff = res_a - res_b;
  EXPECT_TRUE(diff.count() < 0);
}

TEST(Test_Utilities, timepoint_to_string_working)
{
  std::string test_str{ "12:34:56.7" };
  auto res = string_to_timepoint(test_str);
  auto res_str = timepoint_to_string(res);
  EXPECT_EQ(res_str, "12:34:56");
}

TEST(Test_Utilities, render_nonprintable_characters_working)
{
  std::string test_object{ "\tThis is a test string\n" };
  std::string result{ render_nonprintable_characters(test_object) };
  EXPECT_EQ(result, std::string("\\tThis\\sis\\sa\\stest\\sstring\\n"));
}

TEST(Test_Utilities, nearly_equal_working)
{
  auto res{ nearly_equal<double, double>(0.1, 0.1) };
  EXPECT_TRUE(res);
}