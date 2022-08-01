#ifndef THREADRUNNER_HPP
#define THREADRUNNER_HPP

#pragma once

#include <atomic>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

class ThreadWrapper
{
public:
  ThreadWrapper(std::string, std::size_t = 250, std::size_t = 500);
  virtual ~ThreadWrapper();

  void Resume();
  void Pause();

protected:
  virtual void WorkLoop();
  virtual void TestLoop();
  virtual void Work(){};
  virtual void Test(){};

  std::string m_name;
  std::thread m_worker, m_tester;
  std::size_t m_startup_delay, m_pause_delay;
  std::atomic<bool> m_worker_on, m_is_paused;
  std::atomic<std::size_t> m_worker_tick;
};

#endif// THREADRUNNER_HPP