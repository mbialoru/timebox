#include "defines.hpp"

/* Description
==============
Prototype to troubleshoot regex usage
==============
*/

#if PROTO_REGEX

#include <gtest/gtest.h>
#include <regex>

TEST(Proto_Regex, matching_readout)
{
  const std::string regex_pattern{ "^[0-9]{1,2}\\:[0-9]{1,2}\\:[0-9]{1,2}\\.[0-9]{1,2}\\n?$" };
  auto input_strings{ std::to_array<std::string>({ "11:12:13.00\n", "7:12:43.99\n", "1:2:3.0\n", "12:31:11.0" }) };

  std::regex regex_object{ regex_pattern };

  for (const auto &input : input_strings) { EXPECT_TRUE(std::regex_match(input, regex_object)); }
}

#endif