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

template <class T>
PID<T>::PID(double p, double i, double d, T target)
{
  this->kp = p;
  this->ki = i;
  this->kd = d;
  this->pTerm = 0;
  this->iTerm = 0;
  this->dTerm = 0;
  this->lastError = 0;
  this->errorGuard = 20;
  this->target = target;
}

template <class T>
T PID<T>::get_output() const
{
  return this->output;
}

template <class T>
T PID<T>::get_target() const
{
  return target;
}

template <class T>
void PID<T>::set_target(T target)
{
  this->target = target;
}

template <class T>
double PID<T>::get_errorGuard() const
{
  return errorGuard;
}

template <class T>
void PID<T>::set_errorGuard(double errorGuard)
{
  this->errorGuard = errorGuard;
}

template <class T>
void PID<T>::update(T feedback, double timeDelta)
{
  T error = target - feedback;
  T errorDelta = error - lastError;
  lastError = error;
  pTerm = kp * error;
  iTerm += error * timeDelta;

  if (iTerm < -errorGuard)
    iTerm = -errorGuard;
  else if (iTerm > errorGuard)
    iTerm = errorGuard;

  dTerm = errorDelta / timeDelta;
  this->output = pTerm + (ki * iTerm) + (kd * dTerm);
}

#endif // PID_HPP