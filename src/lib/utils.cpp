#include "utils.hpp"

bool UsingSudo()
{
  if (getuid() == geteuid())
    return false;
  return true;
}

bool RunningAsRoot()
{
  if (getuid() == 0 || geteuid() == 0)
    return true;
  return false;
}

bool RunningFromDockerContainer()
{
  if (std::filesystem::exists(std::filesystem::path("/.dockerenv")))
    return true;
  return false;
}