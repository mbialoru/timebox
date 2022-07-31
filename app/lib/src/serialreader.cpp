#include "serialreader.hpp"

SerialReader::SerialReader(const char *tty, std::size_t baud, std::function<void(std::string)> callback)
  : ThreadWrapper::ThreadWrapper("StringReader")
{
  this->callback = callback;
  InitSerial(tty, baud);
  paused = false;
}

void SerialReader::InitSerial(const char *tty, std::size_t baud)
{
  try {
    sp.Open(tty);
    std::this_thread::sleep_for(std::chrono::seconds(this->flush_delay));
    sp.FlushIOBuffers();
  } catch (const LibSerial::OpenFailed &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Failed to open connection on " << tty;
    throw e;
  }
  sp.SetBaudRate(static_cast<LibSerial::BaudRate>(ConvertBaudRate(baud)));
  sp.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  sp.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  sp.SetParity(LibSerial::Parity::PARITY_NONE);
  sp.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
}

void SerialReader::Work()
{
  while (sp.IsDataAvailable()) {
    for (size_t i = 0; i < buffer_size; i++) { serial_buffer[i] = '\0'; }

    for (std::size_t i = 0; i < buffer_size; i++) {
      try {
        sp.ReadByte(serial_buffer[i], read_timeout);
        if (serial_buffer[i] == '\n') break;
      } catch (const LibSerial::ReadTimeout &) {
        BOOST_LOG_TRIVIAL(info) << "Reached port timeout value !";
      }
    }
    callback(std::string(serial_buffer.begin(), serial_buffer.end()));
  }
}

void SerialReader::Test()
{
  thread_local static std::string last_buf{ std::string(std::begin(serial_buffer), std::end(serial_buffer)) };

  thread_local std::string curr_buf{ std::string(std::begin(serial_buffer), std::end(serial_buffer)) };

  if (curr_buf == last_buf && worker_tick > 0) BOOST_LOG_TRIVIAL(error) << "Serial connection error!";

  last_buf = curr_buf;
  std::this_thread::sleep_for(std::chrono::milliseconds(pause_delay * 3));
}