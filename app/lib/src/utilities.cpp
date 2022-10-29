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
#elif defined(_WIN64) && !defined(__CYGWIN__)
void TimeBox::WindowsErrorDebugLog(const char *t_method_name, const char *t_addendum)
{
  std::string error_message{ std::string(t_method_name, " failed: ") };
  auto error_code{ GetLastError() };

  switch (error_code) {
  case ERROR_ACCESS_DENIED:
    error_message.append("Access denied");
    break;
  case ERROR_DATABASE_DOES_NOT_EXIST:
    error_message.append("Database does not exist");
    break;
  case ERROR_INVALID_HANDLE:
    error_message.append("Invalid handle");
    break;
  case ERROR_INVALID_NAME:
    error_message.append("Invalid name");
    break;
  case ERROR_SERVICE_DOES_NOT_EXIST:
    error_message.append("Service does not exist");
    break;
  case ERROR_INSUFFICIENT_BUFFER:
    error_message.append("Insufficient buffer");
    break;
  case ERROR_INVALID_PARAMETER:
    error_message.append("Invalid parameter");
    break;
  case ERROR_INVALID_LEVEL:
    error_message.append("Invalid level");
    break;
  case ERROR_SHUTDOWN_IN_PROGRESS:
    error_message.append("Shutdown in progress");
    break;
  default:
    error_message.append("Unknown error, code: ");
    error_message.append(std::to_string(error_code));
    break;
  }

  if (t_addendum != NULL) {
    error_message.append(" Addendum: ");
    error_message.append(t_addendum);
  }

  BOOST_LOG_TRIVIAL(error) << error_message;
}

TCHAR *GetUsbComPort(TCHAR *t_vid, TCHAR *t_pid)
{
  DWORD device_index{ 0 };
  SP_DEVINFO_DATA device_info_data;
  PCWSTR device_enum{ L"USB" };
  TCHAR expected_id[80]{ 0 };
  BYTE buffer[1024]{ 0 };
  DEVPROPTYPE property_type;
  DWORD size{ 0 };
  DWORD error{ 0 };

  wcscpy_s(expected_id, L"vid_");
  wcscpy_s(expected_id, t_vid);
  wcscpy_s(expected_id, L"&pid_");
  wcscpy_s(expected_id, t_pid);

  auto device_info{ SetupDiGetClassDevs(NULL, device_enum, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT) };

  if (device_info == INVALID_HANDLE_VALUE) { WindowsErrorDebugLog("SetupDiGetClassDevs"); }

  ZeroMemory(&device_info_data, sizeof(SP_DEVINFO_DATA));
  device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

  while (SetupDiEnumDevi) { /* code */ }
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
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };
  if (manager_handle == NULL) { WindowsErrorDebugLog("OpenSCManager"); }

  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_QUERY_STATUS) };
  if (service_handle == NULL) { WindowsErrorDebugLog("OpenService"); }

  SERVICE_STATUS_PROCESS status;
  DWORD bytes_needed{ 0 };
  BOOL result{ QueryServiceStatusEx(
    service_handle, SC_STATUS_PROCESS_INFO, (BYTE *)&status, sizeof(status), &bytes_needed) };
  if (result == 0) { WindowsErrorDebugLog("QueryServiceStatusEx"); }

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
  std::vector<std::string> port_names;
#if defined(__unix__)
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
  UINT available_devices;
  GetRawInputDeviceList(NULL, &available_devices, sizeof(RAWINPUTDEVICELIST));

  if (available_devices == 0) {
    WindowsErrorDebugLog("GetRawInputDeviceList", "No available valid devices found");
    throw ListSerialDevicesError();
  }

  PRAWINPUTDEVICELIST devices_list = new RAWINPUTDEVICELIST[sizeof(RAWINPUTDEVICELIST) * available_devices];
  if (devices_list == NULL) {
    WindowsErrorDebugLog("GetRawInputDeviceList", "Failed to allocate memory for devices list");
    throw ListSerialDevicesError();
  }

  int result{ GetRawInputDeviceList(devices_list, &available_devices, sizeof(RAWINPUTDEVICELIST)) };
  if (result < 0) {
    delete[] devices_list;
    WindowsErrorDebugLog("GetRawInputDeviceList", "Failed to acquire devices list");
    throw ListSerialDevicesError();
  }

  for (UINT i = 0; i < available_devices; i++) {
    UINT buffer_size{ 0 };
    result = GetRawInputDeviceInfo(devices_list[i].hDevice, RIDI_DEVICENAME, NULL, &buffer_size);
    if (result < 0) {
      WindowsErrorDebugLog("GetRawInputDeviceInfo", "Failed to acquire device information, skipping ...");
      continue;
    }

    WCHAR *device_name = new WCHAR[buffer_size + 1];
    if (device_name == NULL) {
      WindowsErrorDebugLog("new operator", "Failed to allocate memory for device name, skipping ...");
      continue;
    }

    result = GetRawInputDeviceInfo(devices_list[i].hDevice, RIDI_DEVICENAME, device_name, &buffer_size);
    if (result < 0) {
      delete[] device_name;
      WindowsErrorDebugLog("GetRawInputDeviceInfo", "Failed to get device name, skipping ...");
      continue;
    }

    RID_DEVICE_INFO rid_device_info;
    rid_device_info.cbSize = sizeof(RID_DEVICE_INFO);
    buffer_size = rid_device_info.cbSize;

    result = GetRawInputDeviceInfo(devices_list[i].hDevice, RIDI_DEVICEINFO, &rid_device_info, &buffer_size);
    if (result < 0) {
      WindowsErrorDebugLog("GetRawInputDeviceInfo", "Failed to get device info, skipping ...");
      continue;
    }
  }
  return port_names;
#endif
}
