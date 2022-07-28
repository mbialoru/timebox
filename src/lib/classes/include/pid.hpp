#ifndef PID_HPP
#define PID_HPP

#pragma once

template <class T>
class PID
{
public:
  PID(double, double, double, T);
  ~PID() = default;

  void set_target(T);
  T get_target() const;

  void set_errorGuard(double);
  double get_errorGuard() const;

  T get_output() const;
  void update(T, double);

private:
  T target;
  T output;
  T lastError;
  double errorGuard;
  double kp, ki, kd;
  double pTerm, iTerm, dTerm;
};

#include "../pid.ipp"

#endif // PID_HPP