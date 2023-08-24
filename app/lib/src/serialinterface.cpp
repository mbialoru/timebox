#include "serialinterface.hpp"

using namespace TimeBox;

SerialInterface::SerialInterface(const std::function<void(TimeboxReadout)> t_callback,
  const std::chrono::milliseconds t_timeout,
  const boost::asio::serial_port_base::parity t_parity,
  const boost::asio::serial_port_base::character_size t_character_size,
  const boost::asio::serial_port_base::flow_control t_flow_control,
  const boost::asio::serial_port_base::stop_bits t_stop_bits)
  : m_callback(std::move(t_callback)), m_timeout(t_timeout), m_parity(t_parity), m_character_size(t_character_size),
    m_flow_control(t_flow_control), m_stop_bits(t_stop_bits), m_port_open(false), m_error_flag(false)
{
  mp_serial_port = std::make_shared<boost::asio::serial_port>(m_io_service);
  std::unique_lock<std::mutex>(m_condition_variable_mutex).swap(m_condition_variable_lock);
}

SerialInterface::~SerialInterface()
{
  m_callback = nullptr;

  if (is_open()) {
    try {
      close();
    } catch (...) {}// Do not throw exceptions in destructor
  }
}

void SerialInterface::close()
{
  if (not is_open()) { return; }

  m_port_open = false;

  m_io_service.post(std::bind(&SerialInterface::close_port, this));

  if (m_worker_thread.joinable()) { m_worker_thread.join(); }

  if (m_callback_thread.joinable()) { m_callback_thread.join(); }

  m_io_service.reset();

  if (error_status()) {
    BOOST_LOG_TRIVIAL(error) << "Error while closing serial port";
    throw(std::system_error(boost::system::error_code()));
  }
}

void SerialInterface::open(const char *tp_device,
  const std::size_t t_baud,
  const std::optional<boost::asio::serial_port_base::parity> to_parity,
  const std::optional<boost::asio::serial_port_base::character_size> to_character_size,
  const std::optional<boost::asio::serial_port_base::flow_control> to_flow_control,
  const std::optional<boost::asio::serial_port_base::stop_bits> to_stop_bits)
{
  std::string device{ tp_device };

  open(device, t_baud, to_parity, to_character_size, to_flow_control, to_stop_bits);
}

void SerialInterface::open(const std::string &tr_device,
  const std::size_t t_baud,
  const std::optional<boost::asio::serial_port_base::parity> to_parity,
  const std::optional<boost::asio::serial_port_base::character_size> to_character_size,
  const std::optional<boost::asio::serial_port_base::flow_control> to_flow_control,
  const std::optional<boost::asio::serial_port_base::stop_bits> to_stop_bits)
{
  if (is_open()) { close(); }

  set_error_status(true);// In case of exception - it stays true

  mp_serial_port->open(tr_device);
  mp_serial_port->set_option(boost::asio::serial_port_base::baud_rate(static_cast<unsigned>(t_baud)));
  mp_serial_port->set_option(to_parity.value_or(m_parity));
  mp_serial_port->set_option(to_character_size.value_or(m_character_size));
  mp_serial_port->set_option(to_flow_control.value_or(m_flow_control));
  mp_serial_port->set_option(to_stop_bits.value_or(m_stop_bits));

  m_io_service.post(std::bind(&SerialInterface::read_begin, this));

  std::thread worker_thread{ std::bind(
    static_cast<std::size_t (boost::asio::io_service::*)()>(&boost::asio::io_service::run), &m_io_service) };

  m_worker_thread.swap(worker_thread);

  std::thread callback_thread{ std::bind(&SerialInterface::callback_loop, this) };

  m_callback_thread.swap(callback_thread);

  set_error_status(false);

  m_port_open = true;
}

// TODO: convert from shared_array to shared_ptr, compile error otherwise

// void SerialInterface::write(const char *tp_data, std::size_t t_buffer_size)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tp_data, tp_data + t_buffer_size);
//   m_io_service.post(std::bind(&SerialInterface::write_begin, this));
// }

// void SerialInterface::write(const std::vector<char> &tr_data)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tr_data.begin(), tr_data.end());
//   m_io_service.post(std::bind(&SerialInterface::write_begin, this));
// }

// void SerialInterface::write_string(const std::string &tr_string)
// {
//   std::scoped_lock<std::mutex> lock(m_write_queue_mutex);
//   m_write_queue.insert(m_write_queue.end(), tr_string.begin(), tr_string.end());
//   m_io_service.post(std::bind(&SerialInterface::write_begin, this));
// }

bool SerialInterface::error_status() const
{
  std::scoped_lock<std::mutex> lock(m_error_mutex);

  return m_error_flag;
}

bool SerialInterface::is_open() const { return m_port_open; }

std::size_t SerialInterface::read(char *tp_buffer, std::size_t t_buffer_size)
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::size_t offset{ std::min(t_buffer_size, m_read_queue.size()) };
  std::vector<char>::iterator iterator{ m_read_queue.begin() + offset };

  std::copy(m_read_queue.begin(), iterator, tp_buffer);

  m_read_queue.erase(m_read_queue.begin(), iterator);

  return offset;
}

std::vector<char> SerialInterface::read()
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::vector<char> read_buffer;

  read_buffer.swap(m_read_queue);

  return read_buffer;
}

std::string SerialInterface::read_string()
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::string read_buffer(m_read_queue.begin(), m_read_queue.end());

  std::vector<char>().swap(m_read_queue);// should be leak free

  return read_buffer;
}

std::string SerialInterface::read_string_until(const std::string &tr_flag)
{
  std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
  std::vector<char>::iterator iterator{ find_in_buffer(m_read_queue, tr_flag) };

  if (iterator == m_read_queue.end()) { return std::string(""); }// NOTE: could throw exception instead of empty string

  std::string result(m_read_queue.begin(), iterator);

  iterator += tr_flag.size();// remove flag from buffer

  m_read_queue.erase(m_read_queue.begin(), iterator);

  return result;
}

void SerialInterface::callback_loop()
{
  std::regex readout_regex{ correct_serial_readout_regex };

  while (is_open()) {
    if (m_condition_variable.wait_for(m_condition_variable_lock, m_timeout) == std::cv_status::timeout and is_open()) {
      BOOST_LOG_TRIVIAL(error) << "Timeout reached, check serial connection";
    } else {
      auto buffer_string{ read_string_until("\n") };
      if (std::regex_search(buffer_string, readout_regex)) {
        try {
          m_callback(TimeboxReadout{ buffer_string, std::chrono::system_clock::now() });
        } catch (const std::bad_function_call &e) {
          BOOST_LOG_TRIVIAL(warning) << "SerialInterface callback call failed";
        }
      } else {
        BOOST_LOG_TRIVIAL(warning) << "Received malformed readout data from serial";
      }
    }
  }
}

void SerialInterface::close_port()
{
  boost::system::error_code error_code;

  mp_serial_port->cancel(error_code);

  if (error_code) {
    set_error_status(true);
    BOOST_LOG_TRIVIAL(error) << "Error canceling serial connection";
  }

  mp_serial_port->close(error_code);

  if (error_code) {
    set_error_status(true);
    BOOST_LOG_TRIVIAL(error) << "Error closing serial connection";
  }
}

void SerialInterface::read_begin()
{
  mp_serial_port->async_read_some(boost::asio::buffer(m_read_buffer, S_READ_BUFFER_SIZE),
    boost::bind(&SerialInterface::read_end,
      this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
}

void SerialInterface::read_end(const boost::system::error_code &tr_error, std::size_t t_bytes)
{
  if (tr_error) {
    if (is_open()) {
      close();
      set_error_status(true);
    }
  } else {
    std::scoped_lock<std::mutex> lock(m_read_queue_mutex);
    m_read_queue.insert(m_read_queue.end(), m_read_buffer.data(), m_read_buffer.data() + t_bytes);
    m_condition_variable.notify_all();
    read_begin();
  }
}

void SerialInterface::set_error_status(const bool t_status)
{
  std::scoped_lock<std::mutex> lock(m_error_mutex);

  m_error_flag = t_status;
}

// void SerialInterface::write_begin()
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
//       boost::bind(&SerialInterface::write_end, this, boost::asio::placeholders::error));
//   }
// }

// void SerialInterface::write_end(const boost::system::error_code &tr_error)
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
//       boost::bind(&SerialInterface::write_end, this, boost::asio::placeholders::error));
//   } else {
//     set_error_status(true);
//     close_port();
//   }
// }

std::vector<char>::iterator SerialInterface::find_in_buffer(std::vector<char> &tr_buffer, const std::string &tr_string)
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