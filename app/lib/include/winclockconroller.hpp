#ifndef WINCLOCKCONROLLER_HPP
#define WINCLOCKCONROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>

#include "clockcontroller.hpp"

namespace TimeBox {

class WinClockConroller final : public ClockController
{
public:
  WinClockConroller(std::size_t);
  ~WinClockConroller() = default;

private:
};

}// namespace TimeBox

#endif// WINCLOCKCONROLLER_HPP