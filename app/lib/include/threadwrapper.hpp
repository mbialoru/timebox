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
  std::string name;
  std::thread worker, tester;
  std::size_t startup_delay, pause_delay;
  std::atomic<bool> worker_on, paused;
  std::atomic<std::size_t> worker_tick;

  virtual void WorkLoop();
  virtual void TestLoop();
  virtual void Work(){};
  virtual void Test(){};
};

#endif// THREADRUNNER_HPP