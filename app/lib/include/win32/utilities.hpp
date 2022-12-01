#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <initguid.h>
#include <windows.h>

#include "utilities_common.hpp"

namespace TimeBox {

void WindowsErrorDebugLog(const char *, const char * = nullptr);
void GetAvailableComPorts();

}// namespace TimeBox

#endif// UTILITIES_HPP