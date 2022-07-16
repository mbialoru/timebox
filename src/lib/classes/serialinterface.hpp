#ifndef SERIALINTEFACE_HPP
#define SERIALINTEFACE_HPP

#pragma once

#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>

class SerialInterface
{
private:
  std::ifstream arduino_in;
  std::ofstream arduino_out;
  std::thread worker;
  std::string cmd, in_str;
  std::unique_ptr<time_t> sys_time{ std::make_unique<time_t>() };
  unsigned long worker_tick{ 0 };
  void WorkerLoop(void(*)());

public:
  SerialInterface(const char*, short, void(*)());
  ~SerialInterface();
};

#endif // SERIALINTEFACE_HPP