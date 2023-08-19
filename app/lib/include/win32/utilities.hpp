#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <initguid.h>
#include <windows.h>

#include "utilities_common.hpp"

namespace TimeBox {

void get_available_com_ports();
void windows_error_debug_log(const char *, const char * = nullptr);

}// namespace TimeBox

#endif// UTILITIES_HPP