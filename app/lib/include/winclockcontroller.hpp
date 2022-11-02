#ifndef WINCLOCKCONTROLLER_HPP
#define WINCLOCKCONTROLLER_HPP

#pragma once

#include "clockcontroller.hpp"
#include <boost/log/trivial.hpp>
#include <windows.h>


namespace TimeBox {

class WinClockController final : public ClockController
{
public:
  WinClockController(std::size_t);
  ~WinClockController();

private:
};

}// namespace TimeBox

#endif// WINCLOCKCONTROLLER_HPP