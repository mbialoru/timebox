#ifndef SERIALCONTROLLER_HPP
#define SERIALCONTROLLER_HPP

#pragma once

#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <functional>
#include <libserial/SerialPort.h>
#include "exceptions.hpp"
#include "utils.hpp"

class SerialController
{
public:
  SerialController(const char*, unsigned, std::function<void()>);
  ~SerialController();
  void Reset();

private:
  static const std::size_t BUFFER_SIZE{ 256 };
  std::chrono::milliseconds worker_delay{ 500 };
  unsigned long worker_tick{ 0 };
  char data_buffer[BUFFER_SIZE];
  std::thread worker, watchdog;
  std::size_t timeout{ 250 };
  LibSerial::SerialPort sp;

  std::atomic<bool> wd_run{ false };
  std::atomic<bool> wt_run{ false };

  void WorkerLoop(std::function<void()>);
  void WatchdogLoop();
};

#endif // SERIALCONTROLLER_HPP