#ifndef SERIALREADER_HPP
#define SERIALREADER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <functional>
#include <libserial/SerialPort.h>

#include "threadwrapper.hpp"
#include "utils.hpp"

class SerialReader : public ThreadWrapper
{
public:
  SerialReader(const char *, std::size_t, std::function<void(std::string)>);
  ~SerialReader() = default;

private:
  std::function<void(std::string)> callback = nullptr;

  const std::size_t flush_delay{ 2 };
  static const std::size_t buffer_size{ 256 };
  static const std::size_t read_timeout{ 250 };
  std::array<char, buffer_size> serial_buffer;

  LibSerial::SerialPort sp;

  void InitSerial(const char *, std::size_t);
  void Work() override;
  void Test() override;
};

#endif// SERIALREADER_HPP