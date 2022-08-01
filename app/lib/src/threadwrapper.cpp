#include "threadwrapper.hpp"

ThreadWrapper::ThreadWrapper(std::string name, std::size_t startupDelay, std::size_t pauseDelay)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating threads for " << name;
  m_is_paused = true;
  m_worker_on = true;
  m_startup_delay = startupDelay;
  m_pause_delay = pauseDelay;
  m_name = name;

  m_worker = std::thread(&ThreadWrapper::WorkLoop, this);
  m_tester = std::thread(&ThreadWrapper::TestLoop, this);
};

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling threads for " << m_name;
  m_worker_on = false;
  if (m_worker.joinable()) m_worker.join();
  if (m_tester.joinable()) m_tester.join();
  BOOST_LOG_TRIVIAL(debug) << "Stopped threads for " << m_name;
}

void ThreadWrapper::WorkLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused) {
      Work();
      m_worker_tick++;
    } else
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
  }
}

void ThreadWrapper::TestLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(m_startup_delay));
  while (m_worker_on) {
    if (not m_is_paused)
      Test();
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
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