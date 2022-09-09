#ifndef SERIALREADER_HPP
#define SERIALREADER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <functional>

#include "threadwrapper.hpp"
#include "utilities.hpp"

namespace TimeBox {

class SerialReader : public ThreadWrapper
{
public:
  SerialReader(std::function<void(TimeboxReadout)>);
  virtual ~SerialReader() = default;
  virtual bool QuerryDevice(std::string) = 0;

protected:
  virtual void InitializeSerial(const char *, std::size_t) = 0;
  void WipeSerialBuffer();

  static constexpr std::size_t m_flush_delay{ 2 };
  static constexpr std::size_t m_buffer_size{ 256 };
  static constexpr std::size_t m_read_timeout{ 250 };
  std::function<void(TimeboxReadout)> m_callback = nullptr;
  std::array<char, m_buffer_size> m_serial_buffer;
};

}// namespace TimeBox

#endif// SERIALREADER_HPP