#ifndef LINSERIALREADER_HPP
#define LINSERIALREADER_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <libserial/SerialPort.h>

#include "serialreader.hpp"

namespace TimeBox {

class LinSerialReader final : public SerialReader
{
public:
  LinSerialReader(const char *, std::size_t, std::function<void(TimeboxReadout)>);
  ~LinSerialReader();
  bool QuerryDevice(std::string) override;

private:
  LibSerial::SerialPort m_serial_port;
  void InitializeSerial(const char *, std::size_t) override;
  void Work() override;
};

}// namespace TimeBox

#endif// LINSERIALREADER_HPP