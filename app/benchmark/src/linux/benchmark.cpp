#include <benchmark/benchmark.h>
#include <limits>

#include "utilities.hpp"

class LinuxClockResolution : public benchmark::Fixture
{
public:
  void SetUp(const benchmark::State &st) override
  {
    m_timespec_history.resize(st.range(0));
    m_diff_history.resize(st.range(0) - 1);
    m_best = std::numeric_limits<double>::infinity();
    m_worst = 0.0;
    m_average = 0.0;
  }

  void TearDown(benchmark::State &st) override
  {
    st.counters["worst"] = m_worst;
    st.counters["best"] = m_best;
    st.counters["average"] = m_average;
    st.counters["preferred"] = m_worst * 1.5;

    std::vector<timespec>().swap(m_timespec_history);
    std::vector<double>().swap(m_diff_history);
  }

protected:
  int m_number_of_cpus;
  clockid_t m_cpu_clock_id;
  std::vector<timespec> m_timespec_history;
  std::vector<double> m_diff_history;
  double m_worst;
  double m_best;
  double m_average;

  void ProbeClock(std::size_t iterations, unsigned mode)
  {
    int err{ clock_getcpuclockid(0, &m_cpu_clock_id) };

    for (std::size_t i = 0; i < iterations; i++) {
      err = clock_gettime(mode, &m_timespec_history.at(i));
      if (err != 0) {
        std::ostringstream error_msg;
        error_msg << "Error number: " << errno;
        throw std::runtime_error(error_msg.str());
      }
    }

    for (std::size_t i = 0; i < iterations - 1; ++i) {
      auto diff = (m_timespec_history.at(i + 1).tv_sec - m_timespec_history.at(i).tv_sec)
                  + (m_timespec_history.at(i + 1).tv_nsec - m_timespec_history.at(i).tv_nsec) / 1000000000.0;
      if (diff > m_worst) { m_worst = diff; }
      if (diff < m_best) { m_best = diff; }
      m_diff_history.at(i) = diff;
    }
  }
};

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_RealTime)(benchmark::State &st)
{
  for (auto _ : st) {
    ProbeClock(st.range(0), 0);
    m_average = ComputeVectorAverage<double>(m_diff_history);
  }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Monotonic)(benchmark::State &st)
{
  for (auto _ : st) {
    ProbeClock(st.range(0), 1);
    m_average = ComputeVectorAverage<double>(m_diff_history);
  }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Process_CpuTime)(benchmark::State &st)
{
  for (auto _ : st) {
    ProbeClock(st.range(0), 2);
    m_average = ComputeVectorAverage<double>(m_diff_history);
  }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Thread_CpuTime)(benchmark::State &st)
{
  for (auto _ : st) {
    ProbeClock(st.range(0), 3);
    m_average = ComputeVectorAverage<double>(m_diff_history);
  }
}

BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_RealTime)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Monotonic)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Process_CpuTime)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Thread_CpuTime)->Arg(10000)->UseRealTime();

BENCHMARK_MAIN();