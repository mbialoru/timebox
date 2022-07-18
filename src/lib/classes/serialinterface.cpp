#include "serialinterface.hpp"

SerialInterface::SerialInterface(const char* tty, unsigned baud,
  std::function<void()> callback)
{
  try
  {
    sp.Open(tty);
  }
  catch (const LibSerial::OpenFailed&)
  {
    std::cerr << "The serial port did not open correctly." << std::endl;
  }

  sp.SetBaudRate(static_cast<LibSerial::BaudRate>(ConvertBaudRate(baud)));
  sp.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  sp.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  sp.SetParity(LibSerial::Parity::PARITY_NONE);
  sp.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  wt_run = true;
  worker = std::thread(&SerialInterface::WorkerLoop, this, callback);
  worker.detach();

  wd_run = true;
  watchdog = std::thread(&SerialInterface::WatchdogLoop, this);
  watchdog.detach();
};

SerialInterface::~SerialInterface()
{
  wt_run = false;
  wd_run = false;
}

void SerialInterface::Reset()
{
  throw NotImplementedException();
}

void SerialInterface::WorkerLoop(std::function<void()> callback)
{
  while (wt_run)
  {
    while (sp.IsDataAvailable())
    {
      for (size_t i = 0; i < BUFFER_SIZE; i++)
        data_buffer[i] = '*';

      for (std::size_t i = 0; i < BUFFER_SIZE; i++)
      {
        try
        {
          sp.ReadByte(data_buffer[i], timeout);
          if (data_buffer[i] == '\n')
            break;
        }
        catch (const LibSerial::ReadTimeout&)
        {
          std::cout << "Reached port timeout value !";
        }
      }
      worker_tick++;
      callback();
    }
    std::this_thread::sleep_for(delay);
  }
}

void SerialInterface::WatchdogLoop()
{
  static std::string last_buf{ std::string(data_buffer, BUFFER_SIZE) };
  while (wd_run)
  {
    if (wt_run)
    {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      std::string curr_buf{ std::string(data_buffer, BUFFER_SIZE) };

      if (curr_buf == last_buf && worker_tick > 0)
        std::cout << "Connection error!" << std::flush;

      last_buf = curr_buf;
    }
  }
}