#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <termios.h>
#include <unistd.h>

bool UsingSudo();
bool RunningAsRoot();
void PrintTimex(timex &);
bool RunningFromDockerContainer();
bool TimeSyncServiceIsRunning();
std::size_t ConvertBaudRate(int);
std::chrono::system_clock::time_point TimepointFromString(std::string);
std::string StringFromTimepoint(std::chrono::system_clock::time_point);

#endif// UTILS_HPP