#ifndef SERIALREADER_HPP
#define SERIALREADER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <functional>
#include <libserial/SerialPort.h>

#include "threadwrapper.hpp"
#include "utilities.hpp"

class SerialReader : public ThreadWrapper
{
public:
  SerialReader(const char *, std::size_t, std::function<void(std::string)>);
  ~SerialReader() = default;

private:
  void InitalizeSerial(const char *, std::size_t);
  void Work() override;
  void Test() override;

  std::function<void(std::string)> m_callback = nullptr;
  const std::size_t m_flush_delay{ 2 };
  static const std::size_t m_buffer_size{ 256 };
  static const std::size_t m_read_timeout{ 250 };
  std::array<char, m_buffer_size> m_serial_buffer;
  LibSerial::SerialPort m_serial_port;
};

#endif// SERIALREADER_HPP