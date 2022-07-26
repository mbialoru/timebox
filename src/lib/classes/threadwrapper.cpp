#include "threadwrapper.hpp"

ThreadWrapper::ThreadWrapper(std::size_t sd = 0, std::size_t pd = 500)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating thread for " << name << " " << id;
  paused = false;
  worker_on = true;
  id = std::this_thread::get_id();

  worker = std::thread(&ThreadWrapper::InnerLoop, this);
};

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling thread for " << name << " " << id;
  worker_on = false;
  if (worker.joinable())
    worker.join();
  BOOST_LOG_TRIVIAL(debug) << "Stopped thread for " << name << " " << id;
}

void ThreadWrapper::InnerLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(startup_delay));
  while (worker_on)
  {
    if (not paused)
    {
      Work();
      Test();
      worker_tick++;
    }
    else {
      std::this_thread::sleep_for(std::chrono::milliseconds(pause_delay));
    }
  }
}

void ThreadWrapper::Pause()
{
  BOOST_LOG_TRIVIAL(debug) << "Pausing thread for " << name << " " << id;
  paused = true;
}

void ThreadWrapper::Resume()
{
  BOOST_LOG_TRIVIAL(debug) << "Resumed thread for " << name << " " << id;
  paused = false;
}