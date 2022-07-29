#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <map>
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>

bool UsingSudo();
bool RunningAsRoot();
void PrintTimex(timex&);
bool RunningFromDockerContainer();
bool TimeSyncServiceIsRunning();
std::size_t ConvertBaudRate(int);
std::chrono::system_clock::time_point TimepointFromString(std::string);
std::string StringFromTimepoint(std::chrono::system_clock::time_point);

#endif // UTILS_HPP