#include "mock_common.hpp"

using namespace TimeBox;

ThreadWrapper::ThreadWrapper(std::string t_name, std::size_t t_startup_delay, std::size_t t_pause_delay)
  : m_name(t_name), m_startup_delay(t_startup_delay), m_pause_delay(t_pause_delay)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating threads for " << t_name;
  m_is_paused = true;
  m_worker_on = true;
  std::unique_lock<std::mutex>(m_mutex).swap(m_lock);

  m_worker = std::thread(&ThreadWrapper::WorkLoop, this);
  m_tester = std::thread(&ThreadWrapper::TestLoop, this);
}

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling threads for " << m_name;
  m_worker_on = false;
  m_conditon_variable.notify_one();
  if (m_worker.joinable()) { m_worker.join(); }
  if (m_tester.joinable()) { m_tester.join(); }
  m_lock.unlock();
  BOOST_LOG_TRIVIAL(debug) << "Stopped threads for " << m_name;
}

void ThreadWrapper::WorkLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused) {
      Work();
      m_worker_tick++;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
    }
  }
}

void ThreadWrapper::TestLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused) {
      Test();
      if (m_conditon_variable.wait_for(m_lock, std::chrono::milliseconds(m_pause_delay * 5))
          == std::cv_status::timeout) {
        BOOST_LOG_TRIVIAL(error) << m_timeout_message;
      };
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
    }
  }
}

void ThreadWrapper::Pause()
{
  BOOST_LOG_TRIVIAL(debug) << "Pausing thread for " << m_name;
  m_is_paused = true;
}

void ThreadWrapper::Resume()
{
  BOOST_LOG_TRIVIAL(debug) << "Resumed thread for " << m_name;
  m_is_paused = false;
}

MockSerialReader::MockSerialReader(std::function<void(TimeboxReadout)> t_callback)
  : ThreadWrapper("MockSerialReader"), m_callback(std::move(t_callback))
{}

MockSerialReader::~MockSerialReader()
{
  m_callback = nullptr;
  Close();
}

void MockSerialReader::Open(const std::string &tr_device, std::size_t t_baudrate)
{
  m_is_paused = false;
  m_port_open = true;
}

void MockSerialReader::Close()
{
  m_is_paused = true;
  m_port_open = false;
}

bool MockSerialReader::IsOpen() const { return m_port_open; }

bool MockSerialReader::ErrorStatus() const { return m_error_flag; }

void MockSerialReader::Work()
{
  auto now = std::chrono::system_clock::now() - std::chrono::milliseconds(200);
  m_callback(TimeboxReadout{ ConvertTimepointToString(now) + ".0", now });
  m_conditon_variable.notify_one();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}