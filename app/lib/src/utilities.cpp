#include "utilities.hpp"
#include "exceptions.hpp"

using namespace TimeBox;

#if defined(__unix__)
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
#endif

bool TimeBox::CheckAdminPrivileges()
{
#if defined(__unix__)
  if (getuid() == 0 || geteuid() == 0) {
    return true;
  } else {
    return false;
  }
#elif defined(_WIN64) && !defined(__CYGWIN__)
  PSID administrator_group{ NULL };
  SID_IDENTIFIER_AUTHORITY nt_authority{ SECURITY_NT_AUTHORITY };
  BOOL result{ AllocateAndInitializeSid(
    &nt_authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administrator_group) };
  BOOL is_user_admin{ false };
  if (result) {
    CheckTokenMembership(NULL, administrator_group, &is_user_admin);
    FreeSid(administrator_group);
    if (is_user_admin) {
      return true;
    } else {
      return false;
    }
  }
  BOOST_LOG_TRIVIAL(error) << "Unable to determine if user is an administrator";
  return false;
#endif
}

bool TimeBox::CheckNTPService()
{
#if defined(__unix__)
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
#elif defined(_WIN64) && !defined(__CYGWIN__)
  // NOTE: AFAIK W32Time is the name of service we are looking for
  BOOL ntp_running{ false };
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) };
  if (manager_handle == NULL) { BOOST_LOG_TRIVIAL(error) << "OpenSCManager failed"; }

  SC_HANDLE service_handle{ OpenService(manager_handle, "W32Time", SC_MANAGER_ALL_ACCESS) };
  if (service_handle == NULL) { BOOST_LOG_TRIVIAL(error) << "OpenService failed"; }

  SERVICE_STATUS_PROCESS status;
  DWORD bytes_needed{ 0 };
  BOOL result{ QueryServiceStatusEx(service_handle, SC_STATUS_PROCESS_INFO, (BYTE *)&status, sizeof(status), &bytes_needed) };
  if (result == 0) { BOOST_LOG_TRIVIAL(error) << "QueryServiceStatusEx failed"; }

  if (status.dwCurrentState == SERVICE_RUNNING) { ntp_running = true; }
  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);
  return ntp_running;
#endif
}

std::size_t TimeBox::ConvertBaudRate(const std::size_t t_baud)
{
  auto search = s_baud_conversion_map.find(static_cast<int>(t_baud));
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
#if defined(__unix__)
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
#elif defined(_WIN64) && !defined(__CYGWIN__)
// TODO: Available Serial (COM) ports detection for Windows
  throw NotImplementedException();
#endif
}
