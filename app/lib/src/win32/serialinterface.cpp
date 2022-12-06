#include "serialinterface.hpp"

using namespace TimeBox;

void SerialInterface::FlushOutputBuffer()
{
  if (0 == PurgeComm(mp_serial_port->lowest_layer().native_handle(), PURGE_TXCLEAR)) {
    BOOST_LOG_TRIVIAL(error) << "Error flushing output serial buffer" << boost::system::error_code();
    WindowsErrorDebugLog("FlushOutputBuffer");
    SetErrorStatus(true);
  }
}

void SerialInterface::FlushInputBuffer()
{
  if (0 == PurgeComm(mp_serial_port->lowest_layer().native_handle(), PURGE_RXCLEAR)) {
    BOOST_LOG_TRIVIAL(error) << "Error flushing input serial buffer" << boost::system::error_code();
    WindowsErrorDebugLog("FlushInputBuffer");
    SetErrorStatus(true);
  }
}

void SerialInterface::FlushIOBuffers()
{
  FlushInputBuffer();
  FlushOutputBuffer();
}