#ifndef BASE_SERVICE_HPP
#define BASE_SERVICE_HPP

#pragma once

// Includes here

namespace TimeBox {

enum ServiceState { RUN, STOP, PAUSE, RESUME, UNKNOWN };

class BaseService
{
public:
  BaseService();
  ~BaseService() = default;

private:
  ServiceState m_current_state;
};

};// namespace TimeBox

#endif// BASE_SERVICE_HPP