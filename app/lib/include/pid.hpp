#ifndef PID_HPP
#define PID_HPP

#pragma once

template<class T> class PID
{
public:
  PID(double, double, double, T);
  ~PID() = default;

  void SetTarget(T);
  T GetTarget() const;

  void SetErrorGuard(double);
  double GetErrorGuard() const;

  T GetOutput() const;
  void Update(T, double);

private:
  T m_target;
  T m_output;
  T m_last_error;
  double m_error_guard;
  double m_kp, m_ki, m_kd;
  double m_pterm, m_iterm, m_dterm;
};

template<class T> PID<T>::PID(double p, double i, double d, T target)
{
  m_kp = p;
  m_ki = i;
  m_kd = d;
  m_pterm = 0;
  m_iterm = 0;
  m_dterm = 0;
  m_last_error = 0;
  m_error_guard = 20;
  m_target = target;
}

template<class T> T PID<T>::GetOutput() const { return this->m_output; }

template<class T> T PID<T>::GetTarget() const { return m_target; }

template<class T> void PID<T>::SetTarget(T target) { this->m_target = target; }

template<class T> double PID<T>::GetErrorGuard() const { return m_error_guard; }

template<class T> void PID<T>::SetErrorGuard(double errorGuard) { this->m_error_guard = errorGuard; }

template<class T> void PID<T>::Update(T feedback, double timeDelta)
{
  T error = m_target - feedback;
  T errorDelta = error - m_last_error;
  m_last_error = error;
  m_pterm = m_kp * error;
  m_iterm += error * timeDelta;

  if (m_iterm < -m_error_guard)
    m_iterm = -m_error_guard;
  else if (m_iterm > m_error_guard)
    m_iterm = m_error_guard;

  m_dterm = errorDelta / timeDelta;
  m_output = m_pterm + (m_ki * m_iterm) + (m_kd * m_dterm);
}

#endif// PID_HPP