#ifndef SERIALINTERFACE_HPP
#define SERIALINTERFACE_HPP

#pragma once

#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <condition_variable>
#include <optional>

#include "utilities.hpp"

namespace TimeBox {

class SerialInterface final : private boost::noncopyable
{
public:
  explicit SerialInterface(std::function<void(TimeboxReadout)>,
    std::chrono::milliseconds = std::chrono::duration<int64_t, std::milli>(5000),
    boost::asio::serial_port_base::parity = boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size = boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control = boost::asio::serial_port_base::flow_control(
      boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits = boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
  virtual ~SerialInterface();

  // NOTE: Boost asio does not provide a high-level api for buffer flushing, we need to access the lower layer with
  // platform specific calls - implementation of this method is thus kept separate
  void close();
  void flush_input_buffer();
  void flush_io_buffers();
  void flush_output_buffer();
  void open(const char*,
    std::size_t,
    std::optional<boost::asio::serial_port_base::parity> = std::nullopt,
    std::optional<boost::asio::serial_port_base::character_size> = std::nullopt,
    std::optional<boost::asio::serial_port_base::flow_control> = std::nullopt,
    std::optional<boost::asio::serial_port_base::stop_bits> = std::nullopt);
  void open(const std::string&,
    std::size_t,
    std::optional<boost::asio::serial_port_base::parity> = std::nullopt,
    std::optional<boost::asio::serial_port_base::character_size> = std::nullopt,
    std::optional<boost::asio::serial_port_base::flow_control> = std::nullopt,
    std::optional<boost::asio::serial_port_base::stop_bits> = std::nullopt);
  void write(const char*, std::size_t);
  void write(const std::vector<char>&);
  void write_string(const std::string &);

  bool error_status() const;
  bool is_open() const;

  std::size_t read(char*, std::size_t);

  std::vector<char> read();

  std::string read_string();
  std::string read_string_until(const std::string &);

  static constexpr std::size_t S_READ_BUFFER_SIZE{ 512 };

private:
  void callback_loop();
  void close_port();
  void read_begin();
  void read_end(const boost::system::error_code &, std::size_t);
  void set_error_status(bool);
  void write_begin();
  void write_end(const boost::system::error_code &);

  static std::vector<char>::iterator find_in_buffer(std::vector<char>&, const std::string&);

  boost::asio::io_service m_io_service;

  boost::asio::serial_port_base::character_size m_character_size;
  boost::asio::serial_port_base::flow_control m_flow_control;
  boost::asio::serial_port_base::parity m_parity;
  boost::asio::serial_port_base::stop_bits m_stop_bits;

  boost::shared_array<char> m_write_buffer;

  mutable std::mutex m_error_mutex;

  std::array<char, S_READ_BUFFER_SIZE> m_read_buffer;

  std::atomic<bool> m_error_flag;
  std::atomic<bool> m_port_open;

  std::chrono::milliseconds m_timeout_duration;

  std::condition_variable m_condition_variable;

  std::function<void(TimeboxReadout)> m_callback;

  std::mutex m_condition_variable_mutex;
  std::mutex m_read_queue_mutex;
  std::mutex m_write_queue_mutex;

  std::shared_ptr<boost::asio::serial_port> mp_serial_port;

  std::size_t m_write_buffer_size;

  std::thread m_callback_thread;
  std::thread m_worker_thread;

  std::unique_lock<std::mutex> m_condition_variable_lock;

  std::vector<char> m_read_queue;
  std::vector<char> m_write_queue;
};

}// namespace TimeBox

#endif// SERIALINTERFACE_HPP