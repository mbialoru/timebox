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
  static constexpr std::string_view GIT_SHORT_SHA{ "d82cd907" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view BUILD_DATE{ "2022-12-06 11:17:40 UTC" };
  static constexpr std::string_view BUILD_HASH{ "72f71364e728eaf4b4abcbbe563ce5d6beba846df3c8a9c394ea42a8522a54fd" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
