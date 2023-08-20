#include "mock_common.hpp"

using namespace TimeBox;

ThreadWrapper::ThreadWrapper(std::string t_name, std::size_t t_startup_delay, std::size_t t_pause_delay)
  : m_name(t_name), m_startup_delay(t_startup_delay), m_pause_delay(t_pause_delay)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating threads for " << t_name;
  m_is_paused = true;
  m_worker_on = true;
  std::unique_lock<std::mutex>(m_mutex).swap(m_lock);

  m_worker = std::thread(&ThreadWrapper::work_loop, this);
  m_tester = std::thread(&ThreadWrapper::test_loop, this);
}

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling threads for " << m_name;
  m_worker_on = false;
  m_condition_variable.notify_one();
  if (m_worker.joinable()) { m_worker.join(); }
  if (m_tester.joinable()) { m_tester.join(); }
  m_lock.unlock();
  BOOST_LOG_TRIVIAL(debug) << "Stopped threads for " << m_name;
}

void ThreadWrapper::work_loop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused) {
      work();
      m_worker_tick++;
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
    }
  }
}

void ThreadWrapper::test_loop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused) {
      test();
      if (m_condition_variable.wait_for(m_lock, std::chrono::milliseconds(m_pause_delay * 5))
          == std::cv_status::timeout) {
        BOOST_LOG_TRIVIAL(error) << m_timeout_message;
      };
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
    }
  }
}

void ThreadWrapper::pause()
{
  BOOST_LOG_TRIVIAL(debug) << "Pausing thread for " << m_name;
  m_is_paused = true;
}

void ThreadWrapper::resume()
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
  close();
}

void MockSerialReader::open(const std::string &tr_device, std::size_t t_baudrate)
{
  m_is_paused = false;
  m_port_open = true;
}

void MockSerialReader::close()
{
  m_is_paused = true;
  m_port_open = false;
}

bool MockSerialReader::is_open() const { return m_port_open; }

bool MockSerialReader::error_status() const { return M_ERROR_FLAG; }

void MockSerialReader::work()
{
  auto now = std::chrono::system_clock::now() - std::chrono::milliseconds(200);
  m_callback(TimeboxReadout{ timepoint_to_string(now) + ".0", now });
  m_condition_variable.notify_one();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}