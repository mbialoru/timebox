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

void PrintTimex(timex& t)
{
  std::cout <<
    "modes: " << t.modes << std::endl <<
    "offset: " << t.offset << std::endl <<
    "freq: " << t.freq << std::endl <<
    "maxerror: " << t.maxerror << std::endl <<
    "esterror: " << t.esterror << std::endl <<
    "status: " << t.status << std::endl <<
    "constant: " << t.constant << std::endl <<
    "precision: " << t.precision << std::endl <<
    "tolerance: " << t.tolerance << std::endl <<
    "timeval_sec: " << t.time.tv_sec << std::endl <<
    "timeval_usec: " << t.time.tv_usec << std::endl <<
    "tick: " << t.tick << std::endl <<
    "ppsfreq: " << t.ppsfreq << std::endl <<
    "jitter: " << t.jitter << std::endl <<
    "shift: " << t.shift << std::endl <<
    "stabil: " << t.stabil << std::endl <<
    "calcnt: " << t.calcnt << std::endl <<
    "errcnt: " << t.errcnt << std::endl <<
    "stbcnt: " << t.stbcnt << std::endl <<
    "tai: " << t.tai << std::endl;
}