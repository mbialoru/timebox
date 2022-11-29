#include "mock.hpp"

using namespace TimeBox;

MockClockController::MockClockController()
  : BaseClockController(500), m_time_adjustment(15600), m_timepoint(std::chrono::system_clock::now()),
    mp_pid(std::move(std::make_shared<PID<double>>(1.0, 0.5, 1.0, 0)))
{
  mp_pid->SetLimits(-1000, 1000);
}

MockClockController::~MockClockController() = default;

void MockClockController::AdjustClock(TimeboxReadout t_readout) {}
