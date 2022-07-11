#ifndef UTILS_HPP
#define UTILS_HPP

#pragma once

#include <unistd.h>
#include <filesystem>

bool UsingSudo();
bool RunningAsRoot();
bool RunningFromDockerContainer();

#endif // UTILS_HPP