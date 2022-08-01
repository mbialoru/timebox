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

bool CheckSudo();
bool CheckAdminPrivileges();
bool CheckIfUsingDockerContainer();
bool CheckNTPService();
void PrintTimex(timex &);
std::size_t ConvertBaudRate(int);
std::chrono::system_clock::time_point ConvertTimepointToString(std::string);
std::string ConvertStringToTimepoint(std::chrono::system_clock::time_point);

#endif// UTILS_HPP