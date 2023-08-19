#include "serialinterface.hpp"

using namespace TimeBox;

void SerialInterface::flush_output_buffer()
{
  if (0 == PurgeComm(mp_serial_port->lowest_layer().native_handle(), PURGE_TXCLEAR)) {
    BOOST_LOG_TRIVIAL(error) << "Error flushing output serial buffer" << boost::system::error_code();
    WindowsErrorDebugLog("flush_output_buffer");
    set_error_status(true);
  }
}

void SerialInterface::flush_input_buffer()
{
  if (0 == PurgeComm(mp_serial_port->lowest_layer().native_handle(), PURGE_RXCLEAR)) {
    BOOST_LOG_TRIVIAL(error) << "Error flushing input serial buffer" << boost::system::error_code();
    WindowsErrorDebugLog("flush_input_buffer");
    set_error_status(true);
  }
}

void SerialInterface::flush_io_buffers()
{
  flush_input_buffer();
  flush_output_buffer();
}