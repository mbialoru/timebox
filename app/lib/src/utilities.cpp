#include "utilities.hpp"

bool CheckSudo()
{
  if (getuid() == geteuid())
    return false;
  else
    return true;
}

bool CheckAdminPrivileges()
{
  if (getuid() == 0 || geteuid() == 0)
    return true;
  else
    return false;
}

bool CheckIfUsingDocker()
{
  if (std::filesystem::exists(std::filesystem::path("/.dockerenv")))
    return true;
  else
    return false;
}

bool CheckNTPService()
{
  // For now it only detects systemd-timesyncd
  char line[100];
  FILE *f = popen("pidof systemd-timesyncd", "r");
  fgets(line, 100, f);
  pid_t pid = strtoul(line, NULL, 10);
  pclose(f);

  if (pid > 0)
    return true;
  else
    return false;
}

std::size_t ConvertBaudRate(int baud)
{
  auto search = s_baud_conversion_map.find(baud);
  if (search != s_baud_conversion_map.end())
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

std::chrono::system_clock::time_point ConvertTimepointToString(std::string time_str)
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

std::string ConvertStringToTimepoint(std::chrono::system_clock::time_point tp)
{
  time_t tmp = std::chrono::system_clock::to_time_t(tp);
  struct tm tm = *std::localtime(&tmp);
  std::string res{ std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec) };

  return res;
}

std::vector<std::string> GetSerialDevicesList()
{
  std::vector<std::string> port_names;

  const std::filesystem::path dev_directory{ "/dev/serial/by-id" };
  try {
    if (!exists(dev_directory)) {
      throw std::runtime_error(dev_directory.generic_string() + " does not exist");
    } else {
      for (auto const &path : std::filesystem::directory_iterator{ dev_directory }) {
        if (is_symlink(path)) {
          auto canonical_path = std::filesystem::canonical(read_symlink(path));
          port_names.push_back(canonical_path.generic_string());
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    throw e;
  }
  std::sort(port_names.begin(), port_names.end());
  return port_names;
}
