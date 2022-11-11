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
  static constexpr std::string_view GIT_SHORT_SHA{ "95669aa8" };
  static constexpr std::string_view BUILD_TYPE{ "Debug" };
  static constexpr std::string_view PLATFORM{ "Windows" };
  static constexpr std::string_view COMPILER{ "GNU" };
  static constexpr std::string_view COMPILER_VERSION{ "12.2.0" };
  static constexpr std::string_view BUILD_DATE{ "2022-11-11 21:41:41 UTC" };
  static constexpr std::string_view BUILD_HASH{ "7677f059d6b6bd7f434adad87d404a9de32770418880f79577c6d82608dc8657" };
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
