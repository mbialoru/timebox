#include "threadwrapper.hpp"

ThreadWrapper::ThreadWrapper(std::string name, std::size_t startupDelay, std::size_t pauseDelay)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating threads for " << name;
  m_is_paused = true;
  m_worker_on = true;
  m_startup_delay = startupDelay;
  m_pause_delay = pauseDelay;
  m_name = name;
  std::unique_lock<std::mutex>(m_mutex).swap(m_lock);

  m_worker = std::thread(&ThreadWrapper::WorkLoop, this);
  m_tester = std::thread(&ThreadWrapper::TestLoop, this);
};

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling threads for " << m_name;
  m_worker_on = false;
  m_conditon_variable.notify_one();
  if (m_worker.joinable()) m_worker.join();
  if (m_tester.joinable()) m_tester.join();
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
    } else
      std::this_thread::sleep_for(std::chrono::milliseconds(m_pause_delay));
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