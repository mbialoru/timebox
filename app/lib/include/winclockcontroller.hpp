#ifndef WINCLOCKCONTROLLER_HPP
#define WINCLOCKCONTROLLER_HPP

#pragma once

#include "clockcontroller.hpp"
#include <boost/log/trivial.hpp>
#include <windows.h>
#include <winsvc.h>


namespace TimeBox {

class WinClockController final : public ClockController
{
public:
  WinClockController(std::size_t);
  ~WinClockController();

private:
  void StartNTPService();
  void PauseNTPService();
};

}// namespace TimeBox

#endif// WINCLOCKCONTROLLER_HPP