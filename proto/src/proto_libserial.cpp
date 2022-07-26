#include "defines.hpp"

/* Description
==============
Prototype to practice and explore usage of libserial to communicate with arduino
==============
*/

#if PROTO_LIBSERIAL

#include <gtest/gtest.h>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>

unsigned long ContentLength(char* buf, std::size_t len)
{
  unsigned short length{ 0 };
  for (std::size_t i = 0; i < len; i++)
  {
    if (buf[i] != '*')
    {
      if (buf[i] != '\n')
        length++;
      else
        return length + 1;
    }
  }
  return length;
}

void CallbackDummy(char* buf, std::size_t len)
{
  auto cl = ContentLength(buf, len);
  if (cl < 7 || cl > 12)
    std::cout << "Invalid Buffer Content ! " << cl << std::endl;
  else
    std::cout << "Callback called" << std::endl;
}

TEST(Proto_LibSerial, read_from_port)
{
  GTEST_SKIP() << "Does this in an instant";
  LibSerial::SerialPort sp;
  sp.Open("/dev/ttyACM0");
  sp.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
  sp.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  sp.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  sp.SetParity(LibSerial::Parity::PARITY_NONE);
  sp.SetStopBits(LibSerial::StopBits::STOP_BITS_1);


  while (!sp.IsDataAvailable())
  {
    sleep(1);
  }

  size_t ms_timeout{ 250 };
  char data_byte;

  try
  {
    sp.ReadByte(data_byte, ms_timeout);
    std::cout << data_byte << std::flush;
  }
  catch (LibSerial::ReadTimeout&)
  {
    std::cerr << "Timeout has been reached" << std::endl;
  }

  sleep(1);

  LibSerial::DataBuffer db;

  try
  {
    sp.Read(db, 0, ms_timeout);
  }
  catch (const LibSerial::ReadTimeout&)
  {
    for (size_t i = 0; i < db.size(); i++)
    {
      std::cout << db.at(i) << std::flush;
    }
    std::cerr << "Timeout has been reached waiting for data" << std::endl;
  }
}

TEST(Proto_LibSerial, read_from_stream)
{
  GTEST_SKIP() << "Works to some extent";
  // Instantiate a SerialStream object.
  LibSerial::SerialStream serial_stream;

  try
  {
    // Open the Serial Port at the desired hardware port.
    serial_stream.Open("/dev/ttyACM0");
  }
  catch (const LibSerial::OpenFailed&)
  {
    std::cerr << "The serial port did not open correctly." << std::endl;
  }

  serial_stream.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
  serial_stream.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  serial_stream.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  serial_stream.SetParity(LibSerial::Parity::PARITY_NONE);
  serial_stream.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  while (serial_stream.rdbuf()->in_avail() == 0)
  {
    usleep(1000);
  }

  char serial_buffer[32];

  for (std::size_t i = 0; i < 20;)
  {
    while (serial_stream.IsDataAvailable())
    {
      for (size_t i = 0; i < 32; i++)
      {
        serial_buffer[i] = '*';
      }

      for (std::size_t i = 0; i < 32; i++)
      {
        serial_stream.get(serial_buffer[i]);
        if (serial_buffer[i] == '\n')
        {
          break;
        }
      }

      // Here check if data makes sense
      unsigned short length{ 0 };
      for (std::size_t i = 0; i < 32; i++)
      {
        if (serial_buffer[i] == '*')
        {
          continue;
        }

        if (serial_buffer[i] != '\n')
        {
          length++;
        }
        else {
          length++;
          break;
        }
      }

      if (length < 7 || length > 12)
        std::cout << "Invalid format" << std::endl;

      for (size_t i = 0; i < 32; i++)
      {
        std::cout << serial_buffer[i];
        if (serial_buffer[i] == '\n')
        {
          std::cout << serial_buffer[i];
          break;
        }
      }

      usleep(1000);
      i++;
    }
  }
}

TEST(Proto_LibSerial, read_with_callback)
{
  // NOTE: First OPEN the stream, then SET its properties !
  char serial_buffer[32];
  LibSerial::SerialStream serial_stream;

  try
  {
    serial_stream.Open("/dev/ttyACM0");
  }
  catch (const LibSerial::OpenFailed&)
  {
    std::cerr << "The serial port did not open correctly." << std::endl;
  }

  serial_stream.SetBaudRate(LibSerial::BaudRate::BAUD_9600);
  serial_stream.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
  serial_stream.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  serial_stream.SetParity(LibSerial::Parity::PARITY_NONE);
  serial_stream.SetStopBits(LibSerial::StopBits::STOP_BITS_1);

  for (std::size_t i = 0; i < 20;)
  {
    while (serial_stream.IsDataAvailable())
    {
      for (size_t i = 0; i < 32; i++)
        serial_buffer[i] = '*';

      for (std::size_t i = 0; i < 32; i++)
      {
        serial_stream.get(serial_buffer[i]);
        if (serial_buffer[i] == '\n')
          break;
      }

      CallbackDummy(serial_buffer, 32);

      usleep(1000);
      i++;
    }
  }
}

#endif