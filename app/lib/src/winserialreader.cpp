#include "winserialreader.hpp"

using namespace TimeBox;

WinSerialReader::WinSerialReader(std::function<void(TimeboxReadout)> t_callback,
  boost::asio::serial_port_base::parity t_parity,
  boost::asio::serial_port_base::character_size t_character_size,
  boost::asio::serial_port_base::flow_control t_flow_control,
  boost::asio::serial_port_base::stop_bits t_stop_bits)
  : m_open(false), m_error_flag(false), m_callback(std::move(t_callback)), m_parity(t_parity),
    m_character_size(t_character_size), m_flow_control(t_flow_control), m_stop_bits(t_stop_bits)
{
  mp_serial_port = std::make_shared<boost::asio::serial_port>(m_io_service);
}

WinSerialReader::~WinSerialReader()
{
  m_callback = nullptr;
  if (IsOpen()) {
    try {
      Close();
    } catch (...) {}
  }
}

void WinSerialReader::Open(const char *t_device,
  std::size_t t_baud,
  std::optional<boost::asio::serial_port_base::parity> t_parity,
  std::optional<boost::asio::serial_port_base::character_size> t_character_size,
  std::optional<boost::asio::serial_port_base::flow_control> t_flow_control,
  std::optional<boost::asio::serial_port_base::stop_bits> t_stop_bits)
{
  std::string device{ t_device };
  Open(device, t_baud, t_parity, t_character_size, t_flow_control, t_stop_bits);
}

void WinSerialReader::Open(const std::string &t_device,
  std::size_t t_baud,
  std::optional<boost::asio::serial_port_base::parity> t_parity,
  std::optional<boost::asio::serial_port_base::character_size> t_character_size,
  std::optional<boost::asio::serial_port_base::flow_control> t_flow_control,
  std::optional<boost::asio::serial_port_base::stop_bits> t_stop_bits)
{
  if (IsOpen()) { Close(); }

  SetErrorStatus(true);// In case of exception - it stays true
  mp_serial_port->open(t_device);
  mp_serial_port->set_option(boost::asio::serial_port_base::baud_rate(t_baud));
  mp_serial_port->set_option(t_parity.value_or(m_parity));
  mp_serial_port->set_option(t_character_size.value_or(m_character_size));
  mp_serial_port->set_option(t_flow_control.value_or(m_flow_control));
  mp_serial_port->set_option(t_stop_bits.value_or(m_stop_bits));

  m_io_service.post(std::bind(&WinSerialReader::ReadBegin, this));
  std::thread thread{ std::bind(
    static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &m_io_service) };
  m_worker_thread.swap(thread);

  SetErrorStatus(false);
  m_open = true;
}

void WinSerialReader::ClosePort()
{
  boost::system::error_code error_code;
  mp_serial_port->cancel(error_code);
  if (error_code) {
    SetErrorStatus(true);
    BOOST_LOG_TRIVIAL(error) << "Error canceling serial connection";
  }
  mp_serial_port->close(error_code);
  if (error_code) {
    SetErrorStatus(true);
    BOOST_LOG_TRIVIAL(error) << "Error closing serial connection";
  }
}

void WinSerialReader::Close()
{
  if (not IsOpen()) { return; }
  m_open = false;
  m_io_service.post(std::bind(&WinSerialReader::ClosePort, this));
  if (m_worker_thread.joinable()) { m_worker_thread.join(); }
  m_io_service.reset();
  if (ErrorStatus()) {
    BOOST_LOG_TRIVIAL(error) << "Error while closing serial port";
    throw(std::system_error(boost::system::error_code()));
  }
}

bool WinSerialReader::IsOpen() const { return m_open; }

void WinSerialReader::SetErrorStatus(bool t_status)
{
  std::lock_guard<std::mutex> lock(m_error_mutex);
  m_error_flag = t_status;
}

bool WinSerialReader::ErrorStatus() const
{
  std::lock_guard<std::mutex> lock_guard(m_error_mutex);
  return m_error_flag;
}

std::size_t WinSerialReader::Read(char *t_data, std::size_t t_buffer_size)
{
  std::lock_guard<std::mutex> lock_guard(m_read_queue_mutex);
  std::size_t offset{ std::min(t_buffer_size, m_read_queue.size()) };
  std::vector<char>::iterator iterator{ m_read_queue.begin() + offset };
  std::copy(m_read_queue.begin(), iterator, t_data);
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return offset;
}

std::vector<char> WinSerialReader::Read()
{
  std::lock_guard<std::mutex> lock_guard(m_read_queue_mutex);
  std::vector<char> read_buffer;
  read_buffer.swap(m_read_queue);
  return read_buffer;
}

std::string WinSerialReader::ReadString()
{
  std::lock_guard<std::mutex> lock_guard(m_read_queue_mutex);
  std::string read_buffer(m_read_queue.begin(), m_read_queue.end());
  m_read_queue.clear();// Might leak memory and require a std::swap()
  return read_buffer;
}

std::string WinSerialReader::ReadStringUntil(const std::string t_flag)
{
  std::lock_guard<std::mutex> lock_guard(m_read_queue_mutex);
  std::vector<char>::iterator iterator{ FindInBuffer(m_read_queue, t_flag) };
  if (iterator == m_read_queue.end()) { return std::string(""); }// could throw exception instead of empty
  std::string result(m_read_queue.begin(), iterator);
  iterator += t_flag.size();// remove flag from buffer
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return result;
}

void WinSerialReader::ReadBegin()
{
  mp_serial_port->async_read_some(boost::asio::buffer(m_read_buffer, read_buffer_size),
    boost::bind(
      &WinSerialReader::ReadEnd, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void WinSerialReader::ReadEnd(const boost::system::error_code &t_error, std::size_t t_bytes)
{
  if (t_error) {
    if (IsOpen()) {
      Close();
      SetErrorStatus(true);
    }
  } else {
    std::lock_guard<std::mutex> lock_guard(m_read_queue_mutex);
    m_read_queue.insert(m_read_queue.end(), m_read_buffer.data(), m_read_buffer.data() + t_bytes);
    ReadBegin();
  }
}

std::vector<char>::iterator WinSerialReader::FindInBuffer(std::vector<char> &t_buffer, const std::string &t_needle)
{
  if (t_needle.size() == 0) { return t_buffer.end(); }
  bool found{ false };
  bool mismatch{ false };
  std::vector<char>::iterator iterator{ t_buffer.begin() };

  while (not found) {
    mismatch = false;
    std::vector<char>::iterator result{ std::find(iterator, t_buffer.end(), t_needle[0]) };
    if (result == t_buffer.end()) { return t_buffer.end(); }

    for (std::size_t i = 0; i < t_needle.size(); i++) {
      std::vector<char>::iterator tmp{ result + i };
      if (result == t_buffer.end()) { return t_buffer.end(); }
      if (t_needle[i] != *tmp) {
        mismatch = true;
        iterator = result + 1;
        break;
      }
    }
    if (mismatch) { continue; }
    return result;
  }
}