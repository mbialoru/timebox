#include "winserialreader.hpp"

using namespace TimeBox;

WinSerialReader::WinSerialReader(const char *t_com, std::size_t t_baud, std::function<void(TimeboxReadout)> t_callback)
  : SerialReader(std::move(t_callback))
{
  m_serial_params.DCBLength = sizeof(m_serial_params);
};

WinSerialReader::InitializeSerial(const char *t_com, std::size_t t_baud)
{
  m_serial_handle = CreateFile(t_com, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  GetCommState(m_serial_handle, &m_serial_params);

  m_serial_params.BaudRate = ConvertBaudRate(t_com);
  m_serial_params.ByteSize = 8;

}