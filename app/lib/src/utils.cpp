#include "utils.hpp"

bool UsingSudo()
{
  if (getuid() == geteuid()) return false;
  return true;
}

bool RunningAsRoot()
{
  if (getuid() == 0 || geteuid() == 0) return true;
  return false;
}

bool RunningFromDockerContainer()
{
  if (std::filesystem::exists(std::filesystem::path("/.dockerenv"))) return true;
  return false;
}

bool TimeSyncServiceIsRunning()
{
  // For now it only detects systemd-timesyncd
  char line[100];
  FILE *f = popen("pidof systemd-timesyncd", "r");
  fgets(line, 100, f);
  pid_t pid = strtoul(line, NULL, 10);
  pclose(f);

  if (pid > 0) return true;
  return false;
}

std::size_t ConvertBaudRate(int baud)
{
  static std::map<int, int> conversion_map{ { 0, B0 },
    { 50, B50 },
    { 75, B75 },
    { 110, B110 },
    { 134, B134 },
    { 150, B150 },
    { 200, B200 },
    { 300, B300 },
    { 600, B600 },
    { 1200, B1200 },
    { 1800, B1800 },
    { 2400, B2400 },
    { 4800, B4800 },
    { 9600, B9600 },
    { 19200, B19200 },
    { 38400, B38400 } };

  auto search = conversion_map.find(baud);
  if (search != conversion_map.end())
    return search->second;
  else
    throw std::invalid_argument("Invalid baud rate !");
}

void PrintTimex(timex &t)
{
  std::cout << "modes: " << t.modes << std::endl
            << "offset: " << t.offset << std::endl
            << "freq: " << t.freq << std::endl
            << "maxerror: " << t.maxerror << std::endl
            << "esterror: " << t.esterror << std::endl
            << "status: " << t.status << std::endl
            << "constant: " << t.constant << std::endl
            << "precision: " << t.precision << std::endl
            << "tolerance: " << t.tolerance << std::endl
            << "timeval_sec: " << t.time.tv_sec << std::endl
            << "timeval_usec: " << t.time.tv_usec << std::endl
            << "tick: " << t.tick << std::endl
            << "ppsfreq: " << t.ppsfreq << std::endl
            << "jitter: " << t.jitter << std::endl
            << "shift: " << t.shift << std::endl
            << "stabil: " << t.stabil << std::endl
            << "calcnt: " << t.calcnt << std::endl
            << "errcnt: " << t.errcnt << std::endl
            << "stbcnt: " << t.stbcnt << std::endl
            << "tai: " << t.tai << std::endl;
}

std::chrono::system_clock::time_point TimepointFromString(std::string time_str)
{
  std::vector<std::string> tmp;
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm = *std::localtime(&now);

  boost::split(tmp, time_str, boost::is_any_of(":."));

  tm.tm_hour = std::stoi(tmp[0]);
  tm.tm_min = std::stoi(tmp[1]);
  tm.tm_sec = std::stoi(tmp[2]);

  auto res = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  res += std::chrono::duration<int, std::milli>(std::stoi(tmp[3]) * 10);

  return res;
}

std::string StringFromTimepoint(std::chrono::system_clock::time_point tp)
{
  time_t tmp = std::chrono::system_clock::to_time_t(tp);
  struct tm tm = *std::localtime(&tmp);
  std::string res{ std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec) };

  return res;
}