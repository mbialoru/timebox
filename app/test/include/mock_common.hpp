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

}// namespace TimeBox

#endif// MOCK_COMMON_HPP