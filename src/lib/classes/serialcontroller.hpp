#ifndef SERIALCONTROLLER_HPP
#define SERIALCONTROLLER_HPP

#pragma once

#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <functional>
#include <boost/log/trivial.hpp>
#include <libserial/SerialPort.h>
#include "exceptions.hpp"
#include "utils.hpp"

class SerialController
{
public:
  SerialController(const char*, unsigned, std::function<void()>);
  ~SerialController();
  std::string getBufferString();
  void Reset();

private:
  static const std::size_t buffer_size{ 256 };
  static const std::size_t read_timeout{ 250 };
  std::chrono::milliseconds worker_delay{ 500 };

  LibSerial::SerialPort sp;
  std::thread worker, watchdog;
  std::array<char, buffer_size> data_buffer;

  std::atomic<bool> wd_run{ false };
  std::atomic<bool> wt_run{ false };
  std::atomic<std::size_t> worker_tick{ 0 };

  void WorkerLoop(std::function<void()>);
  void WatchdogLoop();
};

#endif // SERIALCONTROLLER_HPP