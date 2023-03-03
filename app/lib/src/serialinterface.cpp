#include "serialinterface.hpp"

using namespace TimeBox;

SerialInterface::SerialInterface(std::function<void(TimeboxReadout)> t_callback,
  std::chrono::milliseconds t_timeout,
  boost::asio::serial_port_base::parity t_parity,
  boost::asio::serial_port_base::character_size t_character_size,
  boost::asio::serial_port_base::flow_control t_flow_control,
  boost::asio::serial_port_base::stop_bits t_stop_bits)
  : m_callback(std::move(t_callback)), m_timeout_duration(t_timeout), m_parity(t_parity),
    m_character_size(t_character_size), m_flow_control(t_flow_control), m_stop_bits(t_stop_bits), m_port_open(false),
    m_error_flag(false)
{
  mp_serial_port = std::make_shared<boost::asio::serial_port>(m_io_service);
  std::unique_lock<std::mutex>(m_condition_variable_mutex).swap(m_condition_variable_lock);
}

SerialInterface::~SerialInterface()
{
  m_callback = nullptr;
  if (IsOpen()) {
    try {
      Close();
    } catch (...) {}// Do not throw exceptions in destructor
  }
}

void SerialInterface::Open(const char *tp_device,
  std::size_t t_baud,
  std::optional<boost::asio::serial_port_base::parity> to_parity,
  std::optional<boost::asio::serial_port_base::character_size> to_character_size,
  std::optional<boost::asio::serial_port_base::flow_control> to_flow_control,
  std::optional<boost::asio::serial_port_base::stop_bits> to_stop_bits)
{
  std::string device{ tp_device };
  Open(device, t_baud, to_parity, to_character_size, to_flow_control, to_stop_bits);
}

void SerialInterface::Open(const std::string &tr_device,
  std::size_t t_baud,
  std::optional<boost::asio::serial_port_base::parity> to_parity,
  std::optional<boost::asio::serial_port_base::character_size> to_character_size,
  std::optional<boost::asio::serial_port_base::flow_control> to_flow_control,
  std::optional<boost::asio::serial_port_base::stop_bits> to_stop_bits)
{
  if (IsOpen()) { Close(); }

  SetErrorStatus(true);// In case of exception - it stays true
  mp_serial_port->open(tr_device);
  mp_serial_port->set_option(boost::asio::serial_port_base::baud_rate(static_cast<unsigned>(t_baud)));
  mp_serial_port->set_option(to_parity.value_or(m_parity));
  mp_serial_port->set_option(to_character_size.value_or(m_character_size));
  mp_serial_port->set_option(to_flow_control.value_or(m_flow_control));
  mp_serial_port->set_option(to_stop_bits.value_or(m_stop_bits));

  m_io_service.post(std::bind(&SerialInterface::ReadBegin, this));
  std::thread worker_thread{ std::bind(
    static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &m_io_service) };
  m_worker_thread.swap(worker_thread);

  std::thread callback_thread{ std::bind(&SerialInterface::CallbackLoop, this) };
  m_callback_thread.swap(callback_thread);

  SetErrorStatus(false);
  m_port_open = true;
}

void SerialInterface::ClosePort()
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

void SerialInterface::Close()
{
  if (not IsOpen()) { return; }
  m_port_open = false;
  m_io_service.post(std::bind(&SerialInterface::ClosePort, this));
  if (m_worker_thread.joinable()) { m_worker_thread.join(); }
  if (m_callback_thread.joinable()) { m_callback_thread.join(); }
  m_io_service.reset();
  if (ErrorStatus()) {
    BOOST_LOG_TRIVIAL(error) << "Error while closing serial port";
    throw(std::system_error(boost::system::error_code()));
  }
}

bool SerialInterface::IsOpen() const { return m_port_open; }

void SerialInterface::SetErrorStatus(bool t_status)
{
  std::scoped_lock<std::mutex> lock(m_error_mutex);
  m_error_flag = t_status;
}

bool SerialInterface::ErrorStatus() const
{
  std::scoped_lock<std::mutex> lock(m_error_mutex);
  return m_error_flag;
}

std::size_t SerialInterface::Read(char *tp_buffer, std::size_t t_buffer_size)
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::size_t offset{ std::min(t_buffer_size, m_read_queue.size()) };
  std::vector<char>::iterator iterator{ m_read_queue.begin() + offset };
  std::copy(m_read_queue.begin(), iterator, tp_buffer);
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return offset;
}

std::vector<char> SerialInterface::Read()
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::vector<char> read_buffer;
  read_buffer.swap(m_read_queue);
  return read_buffer;
}

std::string SerialInterface::ReadString()
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::string read_buffer(m_read_queue.begin(), m_read_queue.end());
  std::vector<char>().swap(m_read_queue);// Should be leak free
  return read_buffer;
}

std::string SerialInterface::ReadStringUntil(const std::string &tr_flag)
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::vector<char>::iterator iterator{ FindInBuffer(m_read_queue, tr_flag) };
  if (iterator == m_read_queue.end()) { return std::string(""); }// NOTE: could throw exception instead of empty string
  std::string result(m_read_queue.begin(), iterator);
  iterator += tr_flag.size();// remove flag from buffer
  m_read_queue.erase(m_read_queue.begin(), iterator);
  return result;
}

void SerialInterface::ReadBegin()
{
  mp_serial_port->async_read_some(boost::asio::buffer(m_read_buffer, read_buffer_size),
    boost::bind(
      &SerialInterface::ReadEnd, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void SerialInterface::ReadEnd(const boost::system::error_code &tr_error, std::size_t t_bytes)
{
  if (tr_error) {
    if (IsOpen()) {
      Close();
      SetErrorStatus(true);
    }
  } else {
    std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
    m_read_queue.insert(m_read_queue.end(), m_read_buffer.data(), m_read_buffer.data() + t_bytes);
    m_condition_variable.notify_all();
    ReadBegin();
  }
}

// TODO: Convert from shared_array to shared_ptr, compile error otherwise
// void SerialInterface::Write(const char *tp_data, std::size_t t_buffer_size)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tp_data, tp_data + t_buffer_size);
//   m_io_service.post(std::bind(&SerialInterface::WriteBegin, this));
// }

// void SerialInterface::Write(const std::vector<char> &tr_data)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tr_data.begin(), tr_data.end());
//   m_io_service.post(std::bind(&SerialInterface::WriteBegin, this));
// }

// void SerialInterface::WriteString(const std::string &tr_string)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tr_string.begin(), tr_string.end());
//   m_io_service.post(std::bind(&SerialInterface::WriteBegin, this));
// }

// void SerialInterface::WriteBegin()
// {
//   // Do nothing if writing operation is in progress
//   if (m_write_buffer == 0) {
//     std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//     m_write_buffer_size = m_write_queue.size();
//     m_write_buffer.reset(new char[m_write_queue.size()]);
//     std::copy(m_write_queue.begin(), m_write_queue.end(), m_write_buffer.get());
//     m_write_queue.clear();
//     boost::asio::async_write(mp_serial_port,
//       boost::asio::buffer(m_write_buffer.get(), m_write_buffer_size),
//       boost::bind(&SerialInterface::WriteEnd, this, boost::asio::placeholders::error));
//   }
// }

// void SerialInterface::WriteEnd(const boost::system::error_code &tr_error)
// {
//   if (!tr_error) {
//     std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//     if (m_write_queue.empty()) {
//       m_write_buffer.reset();
//       m_write_buffer_size = 0;
//       return;
//     }
//     m_write_buffer_size = m_write_queue.size();
//     m_write_buffer.reset(new char[m_write_queue.size()]);
//     std::copy(m_write_queue.begin(), m_write_queue.end(), m_write_buffer.get());
//     m_write_queue.clear();
//     boost::asio::async_write(mp_serial_port,
//       boost::asio::buffer(m_write_buffer.get(), m_write_buffer_size),
//       boost::bind(&SerialInterface::WriteEnd, this, boost::asio::placeholders::error));
//   } else {
//     SetErrorStatus(true);
//     ClosePort();
//   }
// }

std::vector<char>::iterator SerialInterface::FindInBuffer(std::vector<char> &tr_buffer, const std::string &tr_string)
{
  if (tr_string.size() == 0) { return tr_buffer.end(); }
  bool mismatch{ false };
  std::vector<char>::iterator iterator{ tr_buffer.begin() };

  while (true) {
    std::vector<char>::iterator result{ std::find(iterator, tr_buffer.end(), tr_string[0]) };
    if (result == tr_buffer.end()) { return tr_buffer.end(); }

    for (std::size_t i = 0; i < tr_string.size(); i++) {
      std::vector<char>::iterator tmp{ result + i };
      if (result == tr_buffer.end()) { return tr_buffer.end(); }
      if (tr_string[i] != *tmp) {
        mismatch = true;
        iterator = result + 1;
        break;
      }
    }
    if (mismatch) { continue; }
    return result;
  }
}

void SerialInterface::CallbackLoop()
{
  std::regex readout_regex{ correct_serial_readout_regex };

  while (IsOpen()) {
    if (m_condition_variable.wait_for(m_condition_variable_lock, m_timeout_duration) == std::cv_status::timeout
        and IsOpen()) {
      BOOST_LOG_TRIVIAL(error) << "Timeout reached, check serial connection";
    } else {
      auto buffer_string{ ReadStringUntil("\n") };
      if (std::regex_search(buffer_string, readout_regex)) {
        m_callback(TimeboxReadout{ buffer_string, std::chrono::system_clock::now() });
      } else {
        BOOST_LOG_TRIVIAL(warning) << "Received malformed readout data from serial";
      }
    }
  }
}