#include "serialreader.hpp"

using namespace TimeBox;

SerialReader::SerialReader(std::function<void(TimeboxReadout)> t_callback)
  : ThreadWrapper("SerialReader"), m_callback(std::move(t_callback)), m_serial_buffer{}
{}

void SerialReader::WipeSerialBuffer()
{
  for (size_t i = 0; i < m_buffer_size; ++i) { m_serial_buffer.at(i) = '\0'; }
}