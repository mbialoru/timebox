#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <memory>
#include <string_view>

namespace TimeBox {

static constexpr std::string_view PROJECT_NAME{ "timebox" };
static constexpr std::string_view PROJECT_VERSION{ "0.1.0" };
static constexpr std::string_view BUILD_INFO{
  "timebox-0.1.0-b401e702-Linux-Debug"
  "-GNU-12.1.0"
};

static constexpr std::size_t WINDOW_HEIGHT{ 600 };
static constexpr std::size_t WINDOW_WIDTH{ 600 };

}// namespace TimeBox

#endif// CONFIG_HPP
