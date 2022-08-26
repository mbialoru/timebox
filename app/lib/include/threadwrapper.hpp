#ifndef THREADRUNNER_HPP
#define THREADRUNNER_HPP

#pragma once

#include <atomic>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

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

}// namespace TimeBox

#endif// THREADRUNNER_HPP