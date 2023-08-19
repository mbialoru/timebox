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

class MockSerialReader final : private ThreadWrapper
{
public:
  explicit MockSerialReader(std::function<void(TimeboxReadout)>);
  virtual ~MockSerialReader();

  void Open(const char *, std::size_t);
  void Open(const std::string &, std::size_t);
  void Close();
  bool is_open() const;
  bool error_status() const;

  void write(const char *, std::size_t){};
  void write(const std::vector<char> &){};
  void write_string(const std::string &){};

  std::vector<char> Read();
  std::size_t Read(char *, std::size_t);
  std::string read_string();
  std::string read_string_until(const std::string);

  static constexpr std::size_t S_READ_BUFFER_SIZE{ 512 };

private:
  void Work() override;
  bool m_port_open;
  const bool m_error_flag{ false };
  std::function<void(TimeboxReadout)> m_callback;
};

}// namespace TimeBox

#endif// MOCK_COMMON_HPP