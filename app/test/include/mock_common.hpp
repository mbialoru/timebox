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

  void pause();
  void resume();

protected:
  virtual void test_loop();
  virtual void test(){};
  virtual void work_loop();
  virtual void work(){};

  std::atomic<bool> m_worker_on, m_is_paused;

  std::atomic<std::size_t> m_worker_tick;

  std::condition_variable m_condition_variable;

  std::mutex m_mutex;

  std::size_t m_startup_delay, m_pause_delay;

  std::string m_name, m_timeout_message{ "Reached watchdog timeout !" };

  std::thread m_worker, m_tester;

  std::unique_lock<std::mutex> m_lock;
};

class MockSerialReader final : private ThreadWrapper
{
public:
  explicit MockSerialReader(std::function<void(TimeboxReadout)>);
  virtual ~MockSerialReader();

  bool error_status() const;
  bool is_open() const;

  void close();
  void open(const char*, std::size_t);
  void open(const std::string &, std::size_t);
  void write_string(const std::string &){};
  void write(const char *, std::size_t){};
  void write(const std::vector<char> &){};

  std::vector<char> read();

  std::size_t read(char*, std::size_t);

  std::string read_string();
  std::string read_string_until(const std::string);

  static constexpr std::size_t S_READ_BUFFER_SIZE{ 512 };

private:
  void work() override;

  bool m_port_open;

  const bool M_ERROR_FLAG{ false };

  std::function<void(TimeboxReadout)> m_callback;
};

}// namespace TimeBox

#endif// MOCK_COMMON_HPP