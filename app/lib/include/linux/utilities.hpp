#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <termios.h>
#include <unistd.h>

#include "utilities_common.hpp"

namespace TimeBox {

bool CheckSudo();
bool CheckIfUsingDocker();

}// namespace TimeBox

#endif// UTILITIES_HPP