#include "utilities.hpp"

using namespace TimeBox;

bool TimeBox::CheckAdminPrivileges()
{
  if (getuid() == 0 || geteuid() == 0) {
    return true;
  } else {
    return false;
  }
}

bool TimeBox::CheckNTPService()
{
  // For now it only detects systemd-timesyncd
  char line[100];
  FILE *f{ popen("pidof systemd-timesyncd", "r") };
  fgets(line, 100, f);
  auto pid{ static_cast<int>(strtol(line, NULL, 10)) };
  pclose(f);

  if (pid > 0) {
    return true;
  } else {
    return false;
  }
}

void TimeBox::PauseNTPService() { throw NotImplementedException(); }

void TimeBox::StartNTPService() { throw NotImplementedException(); }

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

bool TimeBox::CheckSudo()
{
  if (getuid() == geteuid()) {
    return false;
  } else {
    return true;
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