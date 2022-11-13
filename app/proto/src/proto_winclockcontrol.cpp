#include "defines.hpp"

/* Description
==============
Exploring ways to control and modify windows system clock
==============
*/
#if PROTO_WINCLOCKCONTROL
#if defined(_WIN64) && !defined(__CYGWIN__)
#include <boost/log/trivial.hpp>
#include <gtest/gtest.h>
#include <sysinfoapi.h>
#include <windows.h>

#include "clockcontroller.hpp"
#include "utilities.hpp"

namespace TimeBox {

class WinClockController final : public ClockController
{
public:
  WinClockController(const std::size_t t_minimal_delay) : ClockController(t_minimal_delay) {}
  ~WinClockController() = default;

  void AdjustClock(TimeboxReadout) override{};

  void PrototypeTestMethod_A() { GetCurrentClockAdjustments(); }

  void PrototypeTestMethod_B()
  {
    ObtainRequiredPrivileges();
    AdjustSystemClock(500);
  }

private:
  HRESULT ObtainRequiredPrivileges()
  {
    HRESULT hresult;
    HANDLE process_token{ NULL };
    TOKEN_PRIVILEGES token_privileges{ 0 };
    LUID luid;

    if (!LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &luid)) {
      hresult = HRESULT_FROM_WIN32(GetLastError());
      BOOST_LOG_TRIVIAL(error) << "Failed to lookup privilege value " << hresult;
      return hresult;
    }

    // get token for our process
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &process_token)) {
      hresult = HRESULT_FROM_WIN32(GetLastError());
      BOOST_LOG_TRIVIAL(error) << "Failed to open process token " << hresult;
      return hresult;
    }

    // enable SE_SYSTEMTIME_NAME privilege
    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = luid;
    token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(process_token, FALSE, &token_privileges, sizeof(token_privileges), NULL, NULL)) {
      hresult = HRESULT_FROM_WIN32(GetLastError());
      BOOST_LOG_TRIVIAL(error) << "Failed to adjust process token privileges " << hresult;
    } else {
      hresult = S_OK;
      BOOST_LOG_TRIVIAL(info) << "Added SYSTEMTIME privilege to the process token";
    }

    if (process_token != NULL) { CloseHandle(process_token); }

    return hresult;
  }

  void GetCurrentClockAdjustments()
  {
    // NOTE: Precise variant has been introduced with Windows10, MinGW-w64 as of writing this, does __not__ implement
    // precise variant in its sysinfoapi.h (05.10.2022)

    // legacy clock adjustment
    DWORD current_adjustment_legacy{ 0 };
    DWORD time_increment_legacy{ 0 };
    BOOL enabled_legacy{ 0 };
    HRESULT hresult_legacy{ S_OK };

    if (!GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
      hresult_legacy = HRESULT_FROM_WIN32(GetLastError());
    }

    std::stringstream message;
    message << "Adjustment: " << std::to_string(current_adjustment_legacy) << " ";
    message << "Increment: " << std::to_string(time_increment_legacy) << " ";
    message << "Enabled: " << std::to_string(enabled_legacy) << " ";
    message << "HResult: " << std::to_string(hresult_legacy);

    BOOST_LOG_TRIVIAL(info) << message.str();
  }

  void AdjustSystemClock(DWORD t_ppm_adjustment)
  {
    DWORD current_adjustment_legacy{ 0 };
    DWORD time_increment_legacy{ 0 };
    BOOL enabled_legacy{ 0 };
    LARGE_INTEGER perf_counter_frequency{ 0 };
    DWORD new_adjustment_units;
    static constexpr DWORD micro_per_second{ 1000000 };

    if (t_ppm_adjustment > 1000) {
      BOOST_LOG_TRIVIAL(error) << "Too large ppm adjustment";
      return;
    }

    if (!GetSystemTimeAdjustment(&current_adjustment_legacy, &time_increment_legacy, &enabled_legacy)) {
      BOOST_LOG_TRIVIAL(error) << "Failed to read system time adjustment";
      HRESULT_FROM_WIN32(GetLastError());
      return;
    }

    static_cast<void>(QueryPerformanceCounter(&perf_counter_frequency));
    BOOST_LOG_TRIVIAL(info) << "System performance counter frequency "
                            << std::to_string(perf_counter_frequency.QuadPart);

    new_adjustment_units =
      static_cast<DWORD>(static_cast<float>(t_ppm_adjustment * perf_counter_frequency.QuadPart / micro_per_second));

    // Adjust forward
    BOOST_LOG_TRIVIAL(info) << "Adjusting system clock by +" << std::to_string(t_ppm_adjustment) << " PPM (+"
                            << std::to_string(new_adjustment_units) << " adjustment units)";
    if (!SetSystemTimeAdjustment(current_adjustment_legacy + new_adjustment_units, FALSE)) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment " << HRESULT_FROM_WIN32(GetLastError());
      return;
    }

    GetCurrentClockAdjustments();

    // Revert changes
    BOOST_LOG_TRIVIAL(info) << "Reverting changes";
    if (!SetSystemTimeAdjustment(current_adjustment_legacy, FALSE)) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment";
      HRESULT_FROM_WIN32(GetLastError());
    }

    GetCurrentClockAdjustments();

    // Adjust rearward
    BOOST_LOG_TRIVIAL(info) << "Adjusting system clock by -" << std::to_string(t_ppm_adjustment) << " PPM (-"
                            << std::to_string(new_adjustment_units) << " adjustment units)";
    if (!SetSystemTimeAdjustment(current_adjustment_legacy - new_adjustment_units, FALSE)) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment";
      HRESULT_FROM_WIN32(GetLastError());
    }

    GetCurrentClockAdjustments();

    // Revert changes
    BOOST_LOG_TRIVIAL(info) << "Reverting changes";
    if (!SetSystemTimeAdjustment(current_adjustment_legacy, FALSE)) {
      BOOST_LOG_TRIVIAL(error) << "Failed to set system time adjustment";
      HRESULT_FROM_WIN32(GetLastError());
    }
  }
};

}// namespace TimeBox

TEST(Proto_winclockcontrol, printing_system_clock_adjustments)
{
  TimeBox::WinClockController test_object(500);
  test_object.PrototypeTestMethod_A();
}

TEST(Proto_winclockcontrol, setting_system_clock_adjustments)
{
  if (!TimeBox::CheckAdminPrivileges()) { GTEST_SKIP() << "Requires Admin privileges"; }
  TimeBox::WinClockController test_object(500);
  test_object.PrototypeTestMethod_B();
}

#endif
#endif