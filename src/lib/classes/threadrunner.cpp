#include "threadrunner.hpp"

ThreadRunner::ThreadRunner(std::size_t sd = 0, std::size_t pd = 500)
{
  BOOST_LOG_TRIVIAL(debug) << "Creating thread for " << name << " " << id;
  paused = false;
  worker_on = true;
  id = std::this_thread::get_id();

  worker = std::thread(&ThreadRunner::WorkLoop, this);
};

ThreadRunner::~ThreadRunner()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling thread for " << name << " " << id;
  worker_on = false;
}

void ThreadRunner::WorkLoop()
{
  std::this_thread::sleep_for(std::chrono::milliseconds(startup_delay));
  while (worker_on)
  {
    if (not paused)
    {
      DoWork();
      Watchdog();
      worker_tick++;
    }
    else {
      std::this_thread::sleep_for(std::chrono::milliseconds(pause_delay));
    }
  }
}

void ThreadRunner::Pause()
{
  paused = true;
}

void ThreadRunner::Start()
{
  paused = false;
}