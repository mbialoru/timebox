#include "utilities.hpp"
#include "exceptions.hpp"

using namespace TimeBox;

bool TimeBox::CheckSudo()
{
  if (getuid() == geteuid()) {
    return false;
  } else {
    return true;
  }
}

bool TimeBox::CheckAdminPrivileges()
{
  if (getuid() == 0 || geteuid() == 0) {
    return true;
  } else {
    return false;
  }
}

bool TimeBox::CheckIfUsingDocker()
{
  if (std::filesystem::exists(std::filesystem::path("/.dockerenv"))) {
    return true;
  } else {
    return false;
  }
}

bool TimeBox::CheckNTPService()
{
  // For now it only detects systemd-timesyncd
  char line[100];
  FILE *f = popen("pidof systemd-timesyncd", "r");
  fgets(line, 100, f);
  auto pid = static_cast<int>(strtol(line, NULL, 10));
  pclose(f);

  if (pid > 0) {
    return true;
  } else {
    return false;
  }
}

std::size_t TimeBox::ConvertBaudRate(const int t_baud)
{
  auto search = s_baud_conversion_map.find(t_baud);
  if (search != s_baud_conversion_map.end()) {
    return static_cast<std::size_t>(search->second);
  } else {
    throw std::invalid_argument("Invalid baud rate !");
  }
}

std::chrono::system_clock::time_point TimeBox::ConvertStringToTimepoint(const std::string t_time_string)
{
  std::vector<std::string> tmp;
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm = *std::localtime(&now);

  boost::split(tmp, t_time_string, boost::is_any_of(":."));

  tm.tm_hour = std::stoi(tmp[0]);
  tm.tm_min = std::stoi(tmp[1]);
  tm.tm_sec = std::stoi(tmp[2]);

  auto res = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  res += std::chrono::duration<int, std::milli>(std::stoi(tmp[3]) * 10);

  return res;
}

std::string TimeBox::ConvertTimepointToString(const std::chrono::system_clock::time_point t_timepoint)
{
  time_t tmp = std::chrono::system_clock::to_time_t(t_timepoint);
  struct tm tm = *std::localtime(&tmp);
  std::string res{ std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec) };

  return res;
}

std::vector<std::string> TimeBox::GetSerialDevicesList()
{
  std::vector<std::string> port_names;
  const std::filesystem::path dev_directory{ "/dev/serial/by-id" };
  const std::filesystem::path current_directory{ std::filesystem::current_path() };

  try {
    if (!exists(dev_directory)) {
      throw DeviceDirectoryNotExist();
    } else {
      std::filesystem::current_path(dev_directory);
      for (auto const &path : std::filesystem::directory_iterator{ dev_directory }) {
        if (is_symlink(path)) {
          auto canonical_path = std::filesystem::canonical(std::filesystem::read_symlink(path));
          port_names.push_back(canonical_path.generic_string());
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Error scanning for available ports " << e.what();
    throw e;
  }

  std::sort(port_names.begin(), port_names.end());
  std::filesystem::current_path(current_directory);

  return port_names;
}
