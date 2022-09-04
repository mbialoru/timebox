#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <iomanip>
#include <limits>

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

  void SetMask(int t_mask = 0)
  {
    int rc{ 0 };
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(t_mask, &mask);
    m_number_of_cpus = CPU_COUNT(&mask);
    assert((rc = sched_setaffinity(0, sizeof(mask), &mask)) == 0);
  }

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

    double sum{ 0.0 };
    for (std::size_t i = 0; i < m_diff_history.size(); i++) { sum += m_diff_history.at(i); }
    m_average = sum / m_diff_history.size();
  }
};

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_RealTime)(benchmark::State &st)
{
  for (auto _ : st) { ProbeClock(st.range(0), 0); }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Monotonic)(benchmark::State &st)
{
  for (auto _ : st) { ProbeClock(st.range(0), 1); }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Process_CpuTime)(benchmark::State &st)
{
  for (auto _ : st) { ProbeClock(st.range(0), 2); }
}

BENCHMARK_DEFINE_F(LinuxClockResolution, Clock_Thread_CpuTime)(benchmark::State &st)
{
  for (auto _ : st) { ProbeClock(st.range(0), 3); }
}

BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_RealTime)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Monotonic)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Process_CpuTime)->Arg(10000)->UseRealTime();
BENCHMARK_REGISTER_F(LinuxClockResolution, Clock_Thread_CpuTime)->Arg(10000)->UseRealTime();

BENCHMARK_MAIN();