#include "winserialreader.hpp"

using namespace TimeBox;

WinSerialReader::WinSerialReader(const char *t_com, std::size_t t_baud, std::function<void(TimeboxReadout)> t_callback)
  : SerialReader(std::move(t_callback))
{
  m_serial_parameters.DCBlength = sizeof(m_serial_parameters);
};

void WinSerialReader::InitializeSerial(const char *t_com, std::size_t t_baud)
{
  size_t size = { strlen(t_com) };
  wchar_t *warr = new wchar_t[size];
  for (size_t i = 0; i < size; i++) { warr[i] = t_com[i]; }

  m_serial_handle = CreateFile(warr, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  GetCommState(m_serial_handle, &m_serial_parameters);

  m_serial_parameters.BaudRate = ConvertBaudRate(t_baud);
  m_serial_parameters.ByteSize = 8;
}