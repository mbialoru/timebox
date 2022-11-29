#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <initguid.h>
#include <windows.h>

#include "common.hpp"

namespace TimeBox {

std::map<int, int> s_baud_conversion_map{ { 110, CBR_110 },
  { 300, CBR_300 },
  { 600, CBR_600 },
  { 1200, CBR_1200 },
  { 2400, CBR_2400 },
  { 4800, CBR_4800 },
  { 9600, CBR_9600 },
  { 14400, CBR_14400 },
  { 19200, CBR_19200 },
  { 38400, CBR_38400 },
  { 56000, CBR_56000 },
  { 57600, CBR_57600 },
  { 115200, CBR_115200 },
  { 128000, CBR_128000 },
  { 115200, CBR_115200 },
  { 256000, CBR_256000 } };

void WindowsErrorDebugLog(const char *, const char * = nullptr);
void GetAvailableComPorts();

}// namespace TimeBox

#endif// UTILITIES_HPP