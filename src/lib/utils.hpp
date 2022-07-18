#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <filesystem>

struct
{
  int rawrate;
  int termiosrate;
} conversiontable[] =
{
  {0, B0},
  {50, B50},
  {75, B75},
  {110, B110},
  {134, B134},
  {150, B150},
  {200, B200},
  {300, B300},
  {600, B600},
  {1200, B1200},
  {1800, B1800},
  {2400, B2400},
  {4800, B4800},
  {9600, B9600},
  {19200, B19200},
  {38400, B38400}
};

bool UsingSudo();
bool RunningAsRoot();
bool RunningFromDockerContainer();
bool TimeSyncServiceIsRunning();
size_t ConvertBaudRate(int);
void PrintTimex(timex&);

#endif // UTILS_HPP