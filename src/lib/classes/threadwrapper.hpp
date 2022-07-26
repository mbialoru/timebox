#ifndef THREADRUNNER_HPP
#define THREADRUNNER_HPP

#pragma once

#include <memory>
#include <thread>
#include <chrono>
#include <string>
#include <atomic>
#include <boost/log/trivial.hpp>

class ThreadWrapper
{
public:
  ThreadWrapper(std::size_t, std::size_t);
  virtual ~ThreadWrapper();

  void Resume();
  void Pause();

private:
  std::string name;
  std::thread worker;
  std::thread::id id;
  std::size_t startup_delay, pause_delay;
  std::atomic<bool> worker_on, paused;
  std::atomic<std::size_t> worker_tick;

  void InnerLoop();
  virtual void Work() = 0;
  virtual void Test() = 0;
};

#endif // THREADRUNNER_HPP