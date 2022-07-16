#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <unistd.h>
#include <filesystem>

bool UsingSudo();
bool RunningAsRoot();
bool RunningFromDockerContainer();
bool TimeSyncServiceIsRunning();

#endif // UTILS_HPP