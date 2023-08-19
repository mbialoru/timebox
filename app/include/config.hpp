#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdlib>
#include <memory>
#include <string_view>

namespace TimeBox {

struct BuildInformation
{
  static constexpr std::string_view BUILD_DATE{ "2023-04-27 15:26:25 UTC" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view GIT_BRANCH{ "dev" };
  static constexpr std::string_view GIT_SHORT_SHA{ "5cc06140" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view PROJECT_NAME{ "timebox" };
  static constexpr std::string_view PROJECT_VERSION_ADDENDUM{ "InDev" };
  static constexpr std::string_view PROJECT_VERSION{ "0.0.15" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

static constexpr uint32_t MAX_FPS{ 20 };

}// namespace TimeBox

#endif// CONFIG_HPP
