#ifndef WINSERIALREADER_HPP
#define WINSERIALREADER_HPP

#pragma once

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_array.hpp>
#include <windows.h>

#include "utilities.hpp"

namespace TimeBox {

class WinSerialReader final : private boost::noncopyable
{
public:
  WinSerialReader(const char *,
    std::size_t,
    std::function<void(TimeboxReadout)>,
    boost::asio::serial_port_base::parity = boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size = boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control = boost::asio::serial_port_base::flow_control(
      boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits = boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
  WinSerialReader(std::string,
    std::size_t,
    std::function<void(TimeboxReadout)>,
    boost::asio::serial_port_base::parity = boost::asio::serial_port_base::parity(
      boost::asio::serial_port_base::parity::none),
    boost::asio::serial_port_base::character_size = boost::asio::serial_port_base::character_size(8),
    boost::asio::serial_port_base::flow_control = boost::asio::serial_port_base::flow_control(
      boost::asio::serial_port_base::flow_control::none),
    boost::asio::serial_port_base::stop_bits = boost::asio::serial_port_base::stop_bits(
      boost::asio::serial_port_base::stop_bits::one));
  virtual ~WinSerialReader();

  void Open(const std::string &,
    std::size_t,
    boost::asio::serial_port_base::parity,
    boost::asio::serial_port_base::character_size,
    boost::asio::serial_port_base::flow_control,
    boost::asio::serial_port_base::stop_bits);
  void Close();
  bool IsOpen() const;
  bool ErrorStatus() const;

  void Write(const char *, std::size_t);
  void Write(const std::vector<char> &);
  void WriteString(const std::string &);

  std::vector<char> Read();
  std::size_t Read(char *, std::size_t);
  std::string ReadString();
  std::string ReadStringUntil(const std::string);

  static constexpr std::size_t read_buffer_size{ 512 };

private:
  void ClosePort();
  void SetErrorStatus(bool);
  static std::vector<char>::iterator FindInBuffer(std::vector<char> &, const std::string &);

  boost::asio::serial_port_base::parity m_parity;
  boost::asio::serial_port_base::character_size m_character_size;
  boost::asio::serial_port_base::flow_control m_flow_control;
  boost::asio::serial_port_base::stop_bits m_stop_bits;

  boost::asio::io_service m_io_service;
  std::shared_ptr<boost::asio::serial_port> mp_serial_port;
  std::thread m_worker_thread;
  bool m_open;
  bool m_error_flag;
  mutable std::mutex m_error_mutex;

  std::vector<char> m_write_queue;
  boost::shared_array<char> m_write_buffer;
  std::size_t m_write_buffer_size;
  std::mutex m_write_queue_mutex;

  std::array<char, read_buffer_size> m_read_buffer;
  std::vector<char> m_read_queue;
  std::mutex m_read_queue_mutex;

  std::function<void(TimeboxReadout)> m_callback;
};

}// namespace TimeBox

#endif// WINSERIALREADER_HPP