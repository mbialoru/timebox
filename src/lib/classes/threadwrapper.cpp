#include "threadwrapper.hpp"

ThreadWrapper::ThreadWrapper(std::string name, std::size_t sd, std::size_t pd)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating thread for " << name;
  paused = true;
  worker_on = true;
  startup_delay = sd;
  pause_delay = pd;
  this->name = name;

  worker = std::thread(&ThreadWrapper::WorkLoop, this);
  tester = std::thread(&ThreadWrapper::TestLoop, this);
};

ThreadWrapper::~ThreadWrapper()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling thread for " << name;
  worker_on = false;
  if (worker.joinable())
    worker.join();
  if (tester.joinable())
    tester.join();
  BOOST_LOG_TRIVIAL(debug) << "Stopped thread for " << name;
}

void ThreadWrapper::WorkLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(startup_delay));
  while (worker_on)
  {
    if (not paused)
    {
      Work();
      worker_tick++;
    }
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(pause_delay));
  }
}

void ThreadWrapper::TestLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(startup_delay));
  while (worker_on)
  {
    if (not paused)
      Test();
    else
      std::this_thread::sleep_for(std::chrono::milliseconds(pause_delay));
  }
}

void ThreadWrapper::Pause()
{
  BOOST_LOG_TRIVIAL(debug) << "Pausing thread for " << name;
  paused = true;
}

void ThreadWrapper::Resume()
{
  BOOST_LOG_TRIVIAL(debug) << "Resumed thread for " << name;
  paused = false;
}