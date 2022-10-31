#include "winclockcontroller.hpp"

using namespace TimeBox;

WinClockController::WinClockController(const std::size_t t_minimal_delay) : ClockController(t_minimal_delay)
{
  if (CheckNTPService()) {
    try {
      PauseNTPService();
    } catch (const std::exception &e) {
      BOOST_LOG_TRIVIAL(fatal) << e.what();
    }
  }
}

WinClockController::~WinClockController()
{
  if (not CheckNTPService()) {
    try {
      StartNTPService();
    } catch (const std::exception &e) {
      BOOST_LOG_TRIVIAL(fatal) << e.what();
    }
  }
}

void WinClockController::StartNTPService()
{
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };
  if (manager_handle == NULL) { WindowsErrorDebugLog("OpenSCManager"); }
  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_CONTROL_CONTINUE) };
  if (service_handle == NULL) { WindowsErrorDebugLog("OpenService"); }

  SERVICE_CONTROL_STATUS_REASON_PARAMS status;
  BOOL result{ ControlServiceEx(service_handle, SERVICE_CONTROL_CONTINUE, 1, &status) };
  if (result == 0) { WindowsErrorDebugLog("ControlServiceEx"); }

  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);

  if (CheckNTPService()) { throw NTPServiceOperationError("Could not resume NTP service"); }
}

void WinClockController::PauseNTPService()
{
  SC_HANDLE manager_handle{ OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT) };
  if (manager_handle == NULL) { WindowsErrorDebugLog("OpenSCManager"); }
  SC_HANDLE service_handle{ OpenService(manager_handle, L"W32Time", SERVICE_CONTROL_PAUSE) };
  if (service_handle == NULL) { WindowsErrorDebugLog("OpenService"); }

  SERVICE_CONTROL_STATUS_REASON_PARAMS status;
  BOOL result{ ControlServiceEx(service_handle, SERVICE_CONTROL_PAUSE, 1, &status) };
  if (result == 0) { WindowsErrorDebugLog("ControlServiceEx"); }

  CloseServiceHandle(service_handle);
  CloseServiceHandle(manager_handle);

  if (CheckNTPService()) { throw NTPServiceOperationError("Could not pause NTP service"); }
}