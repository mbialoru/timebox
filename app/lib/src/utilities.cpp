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
  BOOST_LOG_TRIVIAL(debug) << "modes: " << t.modes;
  BOOST_LOG_TRIVIAL(debug) << "offset: " << t.offset;
  BOOST_LOG_TRIVIAL(debug) << "freq: " << t.freq;
  BOOST_LOG_TRIVIAL(debug) << "maxerror: " << t.maxerror;
  BOOST_LOG_TRIVIAL(debug) << "esterror: " << t.esterror;
  BOOST_LOG_TRIVIAL(debug) << "status: " << t.status;
  BOOST_LOG_TRIVIAL(debug) << "constant: " << t.constant;
  BOOST_LOG_TRIVIAL(debug) << "precision: " << t.precision;
  BOOST_LOG_TRIVIAL(debug) << "tolerance: " << t.tolerance;
  BOOST_LOG_TRIVIAL(debug) << "timeval_sec: " << t.time.tv_sec;
  BOOST_LOG_TRIVIAL(debug) << "timeval_usec: " << t.time.tv_usec;
  BOOST_LOG_TRIVIAL(debug) << "tick: " << t.tick;
  BOOST_LOG_TRIVIAL(debug) << "ppsfreq: " << t.ppsfreq;
  BOOST_LOG_TRIVIAL(debug) << "jitter: " << t.jitter;
  BOOST_LOG_TRIVIAL(debug) << "shift: " << t.shift;
  BOOST_LOG_TRIVIAL(debug) << "stabil: " << t.stabil;
  BOOST_LOG_TRIVIAL(debug) << "calcnt: " << t.calcnt;
  BOOST_LOG_TRIVIAL(debug) << "errcnt: " << t.errcnt;
  BOOST_LOG_TRIVIAL(debug) << "stbcnt: " << t.stbcnt;
  BOOST_LOG_TRIVIAL(debug) << "tai: " << t.tai;
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
  const std::filesystem::path current_directory{ std::filesystem::current_path() };
  std::filesystem::current_path(dev_directory);

  try {
    if (!exists(dev_directory)) {
      throw std::runtime_error(dev_directory.generic_string() + " does not exist");
    } else {
      for (auto const &path : std::filesystem::directory_iterator{ dev_directory }) {
        if (is_symlink(path)) {
          auto canonical_path = std::filesystem::canonical(std::filesystem::read_symlink(path));
          port_names.push_back(canonical_path.generic_string());
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    throw e;
  }

  std::sort(port_names.begin(), port_names.end());
  std::filesystem::current_path(current_directory);

  return port_names;
}
