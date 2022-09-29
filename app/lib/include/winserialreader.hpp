#ifndef WINSERIALREADER_HPP
#define WINSERIALREADER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <windows.h>

#include "serialreader.hpp"

namespace TimeBox {

class WinSerialReader final : public SerialReader
{
public:
  WinSerialReader();
  ~WinSerialReader() = default;

private:
  HANDLE m_serial_handle;
  DCB m_serial_parameters{ 0 };
  void InitializeSerial(const char *, std::size_t) override;
  void Work() override;
};

}// namespace TimeBox

#endif// WINSERIALREADER_HPP