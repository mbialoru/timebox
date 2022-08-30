#include "serialreader.hpp"

using namespace TimeBox;

SerialReader::SerialReader(const char *t_tty, const std::size_t t_baud, std::function<void(TimeboxReadout)> t_callback)
  : ThreadWrapper::ThreadWrapper("SerialReader"), m_callback(std::move(t_callback)), m_serial_buffer{}
{
  InitalizeSerial(t_tty, t_baud);
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

void SerialReader::InitalizeSerial(const char *t_tty, const std::size_t t_baud)
{
  try {
    m_serial_port.Open(t_tty);
    std::this_thread::sleep_for(std::chrono::seconds(m_flush_delay));
    m_serial_port.FlushIOBuffers();
  } catch (const LibSerial::OpenFailed &e) {
    BOOST_LOG_TRIVIAL(fatal) << "Failed to open connection on " << t_tty;
    throw;
  }

  m_serial_port.SetBaudRate(static_cast<LibSerial::BaudRate>(ConvertBaudRate(static_cast<int>(t_baud))));
  m_serial_port.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  m_serial_port.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  m_serial_port.SetParity(LibSerial::Parity::PARITY_NONE);
  m_serial_port.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
}

void SerialReader::WipeSerialBuffer()
{
  for (size_t i = 0; i < m_buffer_size; ++i) { m_serial_buffer.at(i) = '\0'; }
}

void SerialReader::Work()
{
  while (m_serial_port.IsDataAvailable()) {
    for (std::size_t i = 0; i < m_buffer_size; ++i) {
      try {
        m_serial_port.ReadByte(m_serial_buffer.at(i), m_read_timeout);
        if (m_serial_buffer.at(i) == '\n') { break; }
      } catch (const LibSerial::ReadTimeout &) {
        BOOST_LOG_TRIVIAL(info) << "Reached port timeout value !";
      }
    }
    m_callback(
      TimeboxReadout{ std::string(m_serial_buffer.begin(), m_serial_buffer.end()), std::chrono::system_clock::now() });
    m_conditon_variable.notify_one();
    WipeSerialBuffer();

    // High CPU usage countermeasure
    std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
  }
}