#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <memory>
#include <string_view>

namespace TimeBox {

struct BuildInformation
{
  static constexpr std::string_view PROJECT_NAME{ "timebox" };
  static constexpr std::string_view PROJECT_VERSION{ "0.0.15" };
  static constexpr std::string_view PROJECT_VERSION_ADDENDUM{ "InDev" };
  static constexpr std::string_view GIT_BRANCH{ "dev" };
  static constexpr std::string_view GIT_SHORT_SHA{ "5bab3b31" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view BUILD_DATE{ "2022-11-30 11:33:01 UTC" };
  static constexpr std::string_view BUILD_HASH{ "5e7f27109172d70192cab9e4313c6ad81d087229bb782659ad84c1306b28bf1e" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
