#include "winserialreader.hpp"

using namespace TimeBox;

WinSerialReader::WinSerialReader(const char *t_device,
  std::size_t t_baud,
  std::function<void(TimeboxReadout)> t_callback)
  : WinSerialReader(std::string(t_device), t_baud, t_callback)
{}

WinSerialReader::WinSerialReader(std::string t_device,
  std::size_t t_baud,
  std::function<void(TimeboxReadout)> t_callback)
{
  using namespace boost::asio;
  mp_serial_port = std::make_shared<serial_port>(m_io_service);
  serial_port_base::parity parity{ serial_port_base::parity::none };
  serial_port_base::character_size character_size{ serial_port_base::character_size(8) };
  serial_port_base::flow_control flow_control{ serial_port_base::flow_control::none };
  serial_port_base::stop_bits stop_bits{ serial_port_base::stop_bits(serial_port_base::stop_bits::one) };
}