#ifndef MOCK_COMMON_HPP
#define MOCK_COMMON_HPP

#include <array>
#include <atomic>
#include <boost/log/trivial.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "defines.hpp"
#include "pid.hpp"
#include "utilities.hpp"

namespace TimeBox {

class ThreadWrapper
{
public:
  explicit ThreadWrapper(std::string, std::size_t = 250, std::size_t = 500);
  virtual ~ThreadWrapper();

  void Resume();
  void Pause();

protected:
  virtual void WorkLoop();
  virtual void TestLoop();
  virtual void Work(){};
  virtual void Test(){};

  std::string m_name, m_timeout_message{ "Reached watchdog timeout !" };
  std::thread m_worker, m_tester;
  std::size_t m_startup_delay, m_pause_delay;
  std::mutex m_mutex;
  std::unique_lock<std::mutex> m_lock;
  std::condition_variable m_conditon_variable;
  std::atomic<bool> m_worker_on, m_is_paused;
  std::atomic<std::size_t> m_worker_tick;
};

class MockSerialReader final : public ThreadWrapper
{
public:
  MockSerialReader(std::function<void(TimeboxReadout)>);
  virtual ~MockSerialReader();

  void Open(const char *, std::size_t);
  void Open(const std::string &, std::size_t);
  void Close();
  bool IsOpen() const;
  bool ErrorStatus() const;

  void Write(const char *, std::size_t){};
  void Write(const std::vector<char> &){};
  void WriteString(const std::string &){};

  std::vector<char> Read();
  std::size_t Read(char *, std::size_t);
  std::string ReadString();
  std::string ReadStringUntil(const std::string);

  static constexpr std::size_t read_buffer_size{ 512 };

private:
};

// class MockClockController final : public BaseClockController
// {
// public:
//   MockClockController()
//     : BaseClockController(500), m_tick(10000), m_timepoint(std::chrono::system_clock::now()),
//       mp_pid(std::move(std::make_shared<PID<double>>(1.0, 0.5, 1.0, 0)))
//   {
//     mp_pid->SetLimits(9000, 10000);
//   }

//   ~MockClockController() = default;

//   void AdjustClock(TimeboxReadout t_readout) override
//   {
//     auto [time_string, time_stamp] = t_readout;
//     auto now = std::chrono::system_clock::now();
//     auto last_call_difference = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_call);
//     if (last_call_difference.count() < m_minimal_delay) {
//       BOOST_LOG_TRIVIAL(warning) << "Too soon to last AdjustClock call ! " << last_call_difference.count() << " ms";
//       return;
//     }

//     auto from_str = ConvertStringToTimepoint(time_string);
//     auto diff = now - from_str;
//     time_difference_history.push_back(diff);
//     BOOST_LOG_TRIVIAL(debug) << "Clock difference is " << diff.count() << " nanoseconds";

//     auto processing_time = std::chrono::system_clock::now() - time_stamp;
//     BOOST_LOG_TRIVIAL(debug) << "Processing time was " << processing_time.count() << " nanoseconds";

//     mp_pid->UpdateLimited(static_cast<double>(diff.count()), 1);
//     auto pid_output = mp_pid->GetOutputLimited();
//     auto pid_output_raw = mp_pid->GetOutputRaw();
//     BOOST_LOG_TRIVIAL(debug) << "PID output is " << pid_output;
//     BOOST_LOG_TRIVIAL(debug) << "Raw PID output is " << pid_output_raw;
//     m_tick = pid_output;
//     m_last_call = now;
//   }

//   std::size_t m_tick;
//   std::shared_ptr<PID<double>> mp_pid;
//   std::chrono::system_clock::time_point m_timepoint;
// };

}// namespace TimeBox

#endif// MOCK_COMMON_HPP