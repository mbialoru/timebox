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

bool TimeSyncServiceIsRunning()
{
  // For now it only detects systemd-timesyncd
  char line[100];
  FILE* f = popen("pidof systemd-timesyncd", "r");
  fgets(line, 100, f);
  pid_t pid = strtoul(line, NULL, 10);
  pclose(f);

  if (pid > 0)
    return true;
  return false;
}

size_t ConvertBaudRate(int baud)
{
  for (size_t i = 0; i < sizeof(conversiontable) / sizeof(conversiontable[0]); i++)
  {
    if (conversiontable[i].rawrate == baud)
      return conversiontable[i].termiosrate;
  }
  return -1;
}