#include "utilities.hpp"

using namespace TimeBox;

std::vector<std::size_t> TimeBox::baud_rate_list{ 110,
  300,
  600,
  1200,
  2400,
  4800,
  9600,
  14400,
  19200,
  38400,
  56000,
  57600,
  115200,
  128000,
  115200,
  256000 };

bool TimeBox::check_admin_privileges()
{
  PSID administrator_group{ nullptr };
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

  windows_error_debug_log("AllocateAndInitializeSid");

  return false;
}

bool TimeBox::check_ntp_status()
{
  // NOTE: AFAIK W32Time is the name of service we are looking for
  BOOL ntp_running{ false };
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };

  if (manager_handle == nullptr) { windows_error_debug_log("OpenSCManager"); }

  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_QUERY_STATUS) };

  if (service_handle == nullptr) { windows_error_debug_log("OpenService"); }

  SERVICE_STATUS_PROCESS status;
  DWORD bytes_needed{ 0 };
  BOOL result{ QueryServiceStatusEx(
    service_handle, SC_STATUS_PROCESS_INFO, reinterpret_cast<BYTE *>(&status), sizeof(status), &bytes_needed) };

  if (result == 0) { windows_error_debug_log("QueryServiceStatusEx"); }

  if (status.dwCurrentState == SERVICE_RUNNING) { ntp_running = true; }

  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);

  return ntp_running;
}

std::vector<std::string> TimeBox::get_serial_devices_list()
{
  std::vector<std::string> port_names;
  wchar_t target_path[8192];

  for (int i{ 0 }; i < 255; i++) {// We just simply iterate though all possible COM ports
    std::wstring port_name{ L"COM" + std::to_wstring(i) };
    DWORD query_result{ QueryDosDevice(port_name.c_str(), target_path, 8192) };

    if (query_result != 0) {
      port_names.push_back(std::string(port_name.begin(), port_name.end()));
    } else {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) { windows_error_debug_log("QueryDosDevice"); }
      continue;
    }
  }

  return port_names;
}

void TimeBox::pause_ntp_service()
{
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };

  if (manager_handle == nullptr) { windows_error_debug_log("OpenSCManager"); }

  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_CONTROL_PAUSE) };

  if (service_handle == nullptr) { windows_error_debug_log("OpenService"); }

  SERVICE_CONTROL_STATUS_REASON_PARAMS status;
  BOOL result{ ControlServiceEx(service_handle, SERVICE_CONTROL_PAUSE, 1, &status) };

  if (result == 0) { windows_error_debug_log("ControlServiceEx"); }

  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);

  if (check_ntp_status()) { throw NTPServiceOperationError("Could not pause NTP service"); }
}

void TimeBox::start_ntp_service()
{
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };

  if (manager_handle == nullptr) { windows_error_debug_log("OpenSCManager"); }

  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_CONTROL_CONTINUE) };

  if (service_handle == nullptr) { windows_error_debug_log("OpenService"); }

  SERVICE_CONTROL_STATUS_REASON_PARAMS status;
  BOOL result{ ControlServiceEx(service_handle, SERVICE_CONTROL_CONTINUE, 1, &status) };

  if (result == 0) { windows_error_debug_log("ControlServiceEx"); }

  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);

  if (check_ntp_status()) { throw NTPServiceOperationError("Could not resume NTP service"); }
}

void TimeBox::get_available_com_ports()
{
  wchar_t target_path[10000];

  for (int i{ 0 }; i < 255; i++) {
    std::wstring port_name{ L"COM" + std::to_wstring(i) };
    DWORD query_result{ QueryDosDevice(port_name.c_str(), target_path, 10000) };
    if (query_result != 0) { BOOST_LOG_TRIVIAL(debug) << port_name; }
  }
}

void TimeBox::windows_error_debug_log(const char* tp_method_name, const char* tp_addendum)
{
  std::string error_message{ std::string(tp_method_name, " failed: ") };
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

  if (tp_addendum != nullptr) {
    error_message.append(" Addendum: ");
    error_message.append(tp_addendum);
  }

  BOOST_LOG_TRIVIAL(error) << error_message;
}