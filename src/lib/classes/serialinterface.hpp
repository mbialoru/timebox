#ifndef SERIALINTEFACE_HPP
#define SERIALINTEFACE_HPP

#pragma once

#include <functional>
#include <fstream>
#include <thread>

class SerialInterface
{
private:
  std::ifstream arduino_in;
  std::ofstream arduino_out;
  std::thread worker;
  std::string cmd, in_str;
  std::unique_ptr<time_t> sys_time{ std::make_unique<time_t>() };
  unsigned long worker_tick{ 0 };
  void WorkerLoop(std::function<void()> cb);

public:
  SerialInterface(const char*, short, std::function<void()>);
  ~SerialInterface();
};

#endif // SERIALINTEFACE_HPP