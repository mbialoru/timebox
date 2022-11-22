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
  : m_open(false), m_error_flag(false), m_callback(std::move(t_callback))
{
  using namespace boost::asio;
  mp_serial_port = std::make_shared<serial_port>(m_io_service);
  serial_port_base::parity parity{ serial_port_base::parity::none };
  serial_port_base::character_size character_size{ serial_port_base::character_size(8) };
  serial_port_base::flow_control flow_control{ serial_port_base::flow_control::none };
  serial_port_base::stop_bits stop_bits{ serial_port_base::stop_bits(serial_port_base::stop_bits::one) };
}

std::size_t WinSerialReader::Read(char *t_data, std::size_t t_buffer_size)
{
  std::lock_guard<std::mutex> lock(m_read_queue_mutex);
  std::size_t offset{ std::min(t_buffer_size, m_read_queue.size()) };
  std::vector<char>::iterator iterator{ m_read_queue.begin() + offset };
  std::copy(m_read_queue.begin(), iterator, t_data);
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return offset;
}

std::vector<char> WinSerialReader::Read()
{
  std::lock_guard<std::mutex> lock(m_read_queue_mutex);
  std::vector<char> read_buffer;
  read_buffer.swap(m_read_queue);
  return read_buffer;
}

std::string WinSerialReader::ReadString()
{
  std::lock_guard<std::mutex> lock(m_read_queue_mutex);
  std::string read_buffer(m_read_queue.begin(), m_read_queue.end());
  m_read_queue.clear();// Might leak memory and require a std::swap()
  return read_buffer;
}

std::string WinSerialReader::ReadStringUntil(const std::string t_flag)
{
  std::lock_guard<std::mutex> lock(m_read_queue_mutex);
  std::vector<char>::iterator iterator{ FindInBuffer(m_read_queue, t_flag) };
  if (iterator == m_read_queue.end()) { return std::string(""); }// could throw exception instead of empty
  std::string result(m_read_queue.begin(), iterator);
  iterator += t_flag.size();// remove flag from buffer
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return result;
}

std::vector<char>::iterator WinSerialReader::FindInBuffer(std::vector<char> &t_buffer, const std::string &t_needle)
{
  if (t_needle.size() == 0) { return t_buffer.end(); }
  bool found{ false };
  std::vector<char>::iterator iterator{ t_buffer.begin() };
  while (not found) {
    std::vector<char>::iterator result{ std::find(iterator, t_buffer.end(), t_needle[0]) };
    if (result == t_buffer.end()) { return t_buffer.end(); }

    for (std::size_t i = 0; i < t_needle.size(); i++) {
      std::vector<char>::iterator tmp{ result + i };
      if (result == t_buffer.end()) { return t_buffer.end(); }
      if (t_needle[i] != *tmp) { goto mismatch; }
      // TODO: Figure this out to refactor out goto statement
    }
    return result;
  mismatch:
    iterator = result + 1;
  }
}