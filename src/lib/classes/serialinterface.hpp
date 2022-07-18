#ifndef SERIALINTEFACE_HPP
#define SERIALINTEFACE_HPP

#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include <termios.h>
#include <functional>
#include <libserial/SerialPort.h>
#include "exceptions.hpp"
#include "utils.hpp"

class SerialInterface
{
public:
  SerialInterface(const char*, unsigned, std::function<void()>);
  ~SerialInterface();
  void Reset();

private:
  static const std::size_t BUFFER_SIZE{ 256 };
  std::chrono::milliseconds delay{ 500 };
  unsigned long worker_tick{ 0 };
  char data_buffer[BUFFER_SIZE];
  std::thread worker, watchdog;
  std::size_t timeout{ 250 };
  LibSerial::SerialPort sp;
  bool wd_run{ false };
  bool wt_run{ false };

  void WorkerLoop(std::function<void()>);
  void WatchdogLoop();
};

#endif // SERIALINTEFACE_HPP