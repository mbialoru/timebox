#ifndef PID_HPP
#define PID_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <utility>

template<class T> class PID
{
public:
  PID(double, double, double, T);
  ~PID() = default;

  T GetTarget() const;
  void SetTarget(T);

  double GetErrorGuard() const;
  void SetErrorGuard(double);

  T GetOutputRaw() const;
  T GetOutputLimited() const;

  std::pair<T, T> GetLimits() const;
  void SetLimits(T, T);

  void UpdateRaw(T, double);
  void UpdateLimited(T, double);

private:
  T m_target;
  T m_output;
  T m_last_error;
  T m_lower_limit;
  T m_upper_limit;
  T m_middle_limit;
  T m_limit_difference;
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
  m_lower_limit = static_cast<T>(0);
  m_upper_limit = static_cast<T>(0);
}

template<class T> T PID<T>::GetTarget() const { return m_target; }

template<class T> void PID<T>::SetTarget(T target) { m_target = target; }

template<class T> double PID<T>::GetErrorGuard() const { return m_error_guard; }

template<class T> void PID<T>::SetErrorGuard(double errorGuard) { m_error_guard = errorGuard; }

template<class T> std::pair<T, T> PID<T>::GetLimits() const { return std::pair<T, T>{ m_upper_limit, m_lower_limit }; }

template<class T> void PID<T>::SetLimits(T lowerLimit, T upperLimit)
{
  m_lower_limit = lowerLimit;
  m_upper_limit = upperLimit;
  m_limit_difference = (m_upper_limit - m_lower_limit) / 2;
  m_middle_limit = m_lower_limit + m_limit_difference;
}

template<class T> T PID<T>::GetOutputRaw() const { return m_output; }

template<class T> T PID<T>::GetOutputLimited() const
{
  if (m_upper_limit == static_cast<T>(0) && m_lower_limit == static_cast<T>(0)) {
    BOOST_LOG_TRIVIAL(warning) << "Using limited PID output when limits are not set !";
    return m_output;
  }

  if (m_output + m_middle_limit > m_upper_limit) {
    return m_upper_limit;
  } else if (m_output + m_middle_limit < m_lower_limit) {
    return m_lower_limit;
  } else {
    return m_output + m_middle_limit;
  }
}

template<class T> void PID<T>::UpdateRaw(T feedback, double timeDelta)
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

template<class T> void PID<T>::UpdateLimited(T feedback, double timeDelta)
{
  if (feedback < (-m_limit_difference)) {
    feedback = -m_limit_difference;
  } else if (feedback > (m_limit_difference)) {
    feedback = m_limit_difference;
  }

  UpdateRaw(feedback, timeDelta);
}

#endif// PID_HPP