#include "serialinterface.hpp"

SerialInterface::SerialInterface(const char* tty, short baud, void(*cb)())
{
  // Check if shell is available
  if (!system(NULL))
    exit(EXIT_FAILURE);

  cmd = "stty -F " + std::string(tty) + " cs8 " + std::to_string(baud) + \
    " ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo \
    -echoe -echok -echoctl -echoke noflsh -ixon -crtscts";

  if (system(cmd.c_str()) != 0)
    exit(EXIT_FAILURE);

  arduino_in = std::ifstream(tty);
  arduino_out = std::ofstream(tty);

  std::time(sys_time.get());

  worker = std::thread(&SerialInterface::WorkerLoop, this, cb);
  worker.detach();
};

SerialInterface::~SerialInterface()
{
  arduino_in.close();
  arduino_out.close();
}

void SerialInterface::WorkerLoop(void(*cb)())
{
  std::time(sys_time.get());
  while (!arduino_in.eof())
  {
    arduino_in >> in_str;
    worker_tick++;
    cb();
  }
  arduino_in.clear();
}