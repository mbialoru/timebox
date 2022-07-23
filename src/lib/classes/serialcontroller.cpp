#include "serialcontroller.hpp"

SerialController::SerialController(const char* tty, unsigned baud,
  std::function<void()> callback)
{
  try
  {
    sp.Open(tty);
  }
  catch (const LibSerial::OpenFailed& e)
  {
    BOOST_LOG_TRIVIAL(fatal) << "Failed to open connection on " << tty;
    throw e;
  }

  sp.SetBaudRate(static_cast<LibSerial::BaudRate>(ConvertBaudRate(baud)));
  sp.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  sp.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  sp.SetParity(LibSerial::Parity::PARITY_NONE);
  sp.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  BOOST_LOG_TRIVIAL(debug) << "Creating SerialController worker thread";
  wt_run = true;
  worker = std::thread(&SerialController::WorkerLoop, this, callback);

  BOOST_LOG_TRIVIAL(debug) << "Creating SerialController watchdog thread";
  wd_run = true;
  watchdog = std::thread(&SerialController::WatchdogLoop, this);
};

SerialController::~SerialController()
{
  BOOST_LOG_TRIVIAL(debug) << "Cancelling SerialController worker thread";
  wt_run = false;
  worker.join();

  BOOST_LOG_TRIVIAL(debug) << "Cancelling SerialController watchdog thread";
  wd_run = false;
  watchdog.join();
}

void SerialController::Reset()
{
  throw NotImplementedException();
}

void SerialController::WorkerLoop(std::function<void()> callback)
{
  while (wt_run)
  {
    while (sp.IsDataAvailable())
    {
      for (size_t i = 0; i < buffer_size; i++)
        data_buffer[i] = '\0';

      for (std::size_t i = 0; i < buffer_size; i++)
      {
        try
        {
          sp.ReadByte(data_buffer[i], read_timeout);
          if (data_buffer[i] == '\n')
            break;
        }
        catch (const LibSerial::ReadTimeout&)
        {
          BOOST_LOG_TRIVIAL(info) << "Reached port timeout value !";
        }
      }
      worker_tick++;
      callback();
    }
    std::this_thread::sleep_for(worker_delay);
  }
}

void SerialController::WatchdogLoop()
{
  static std::string last_buf{ std::string(std::begin(data_buffer),
    std::end(data_buffer)) };

  while (wd_run)
  {
    if (wt_run)
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      std::string curr_buf{ std::string(std::begin(data_buffer),
        std::end(data_buffer)) };

      if (curr_buf == last_buf && worker_tick > 0)
        BOOST_LOG_TRIVIAL(error) << "Connection error!";

      last_buf = curr_buf;
    }
  }
}