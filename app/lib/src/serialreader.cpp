#include "serialreader.hpp"

SerialReader::SerialReader(const char *tty, std::size_t baud, std::function<void(std::string)> callback)
  : ThreadWrapper::ThreadWrapper("SerialReader")
{
  m_callback = callback;
  InitalizeSerial(tty, baud);
  WipeSerialBuffer();
  m_is_paused = false;
}

SerialReader::~SerialReader()
{
  WipeSerialBuffer();
  m_callback = nullptr;
  m_is_paused = true;
  m_serial_port.Close();
}

void SerialReader::InitalizeSerial(const char *tty, std::size_t baud)
{
  try {
    m_serial_port.Open(tty);
    std::this_thread::sleep_for(std::chrono::seconds(m_flush_delay));
    m_serial_port.FlushIOBuffers();
  } catch (const LibSerial::OpenFailed &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Failed to open connection on " << tty;
    throw e;
  }

  m_serial_port.SetBaudRate(static_cast<LibSerial::BaudRate>(ConvertBaudRate(baud)));
  m_serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  m_serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  m_serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
  m_serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
}

void SerialReader::WipeSerialBuffer()
{
  for (size_t i = 0; i < m_buffer_size; i++) { m_serial_buffer[i] = '\0'; }
}

void SerialReader::Work()
{
  while (m_serial_port.IsDataAvailable()) {
    for (std::size_t i = 0; i < m_buffer_size; i++) {
      try {
        m_serial_port.ReadByte(m_serial_buffer[i], m_read_timeout);
        if (m_serial_buffer[i] == '\n') break;
      } catch (const LibSerial::ReadTimeout &) {
        BOOST_LOG_TRIVIAL(info) << "Reached port timeout value !";
      }
    }
    m_callback(std::string(m_serial_buffer.begin(), m_serial_buffer.end()));
    m_conditon_variable.notify_one();
    WipeSerialBuffer();
  }
}