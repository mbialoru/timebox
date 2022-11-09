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
  static constexpr std::string_view GIT_BRANCH{ "dev-windows" };
  static constexpr std::string_view GIT_SHORT_SHA{ "b005f238" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view BUILD_DATE{ "2022-11-09 12:39:30 UTC" };
  static constexpr std::string_view BUILD_HASH{ "4c310d35883c445250e09292b9d629ed330b3bf3b909fd20d9547345dc4bfebc" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
