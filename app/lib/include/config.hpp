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
  static constexpr std::string_view GIT_SHORT_SHA{ "fac505cc" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view BUILD_DATE{ "2022-11-27 14:54:34 UTC" };
  static constexpr std::string_view BUILD_HASH{ "d7ebd4512f0c38aac2624cbc136a59d879badde61baa8cefef9136b8c1628bf5" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
