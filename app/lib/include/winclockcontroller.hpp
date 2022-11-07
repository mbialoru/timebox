#ifndef WINCLOCKCONTROLLER_HPP
#define WINCLOCKCONTROLLER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <windows.h>

#include "clockcontroller.hpp"

namespace TimeBox {

class WinClockController final : public ClockController
{
public:
  WinClockController(std::size_t);
  ~WinClockController();

  void AdjustClock(TimeboxReadout) override;

private:
};

}// namespace TimeBox

#endif// WINCLOCKCONTROLLER_HPP