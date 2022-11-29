#ifndef TEST_FAKES_HPP
#define TEST_FAKES_HPP

#include "mock_common.hpp"

class MockClockController final : public BaseClockController
{
public:
  MockClockController();
  ~MockClockController();

  void AdjustClock(TimeboxReadout) override;

private:
  std::size_t m_tick;
  std::shared_ptr<PID<double>> mp_pid;
  std::chrono::system_clock::time_point m_timepoint;
};

#endif// TEST_FAKES_HPP