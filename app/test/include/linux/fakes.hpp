#include "commonfakes.hpp"

class FakeClockController final : public BaseClockController
{
public:
  FakeClockController();
  ~FakeClockController();

  void AdjustClock(TimeboxReadout) override;

private:
  std::size_t m_tick;// for linux
  std::shared_ptr<PID<double>> mp_pid;
  std::chrono::system_clock::time_point m_timepoint;
};