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
  SerialReader(const char *, std::size_t, std::function<void(TimeboxReadout)>);
  ~SerialReader();

private:
  void InitalizeSerial(const char *, std::size_t);
  void WipeSerialBuffer();
  void Work() override;

  static constexpr std::size_t m_flush_delay{ 2 };
  static constexpr std::size_t m_buffer_size{ 256 };
  static constexpr std::size_t m_read_timeout{ 250 };
  std::function<void(TimeboxReadout)> m_callback = nullptr;
  std::array<char, m_buffer_size> m_serial_buffer;
  LibSerial::SerialPort m_serial_port;
};

#endif// SERIALREADER_HPP