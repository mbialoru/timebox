#ifndef PID_HPP
#define PID_HPP

#pragma once

#include <boost/log/trivial.hpp>

namespace TimeBox {

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

template<class T> PID<T>::PID(const double t_p, const double t_i, const double t_d, const T t_target)
{
  m_kp = t_p;
  m_ki = t_i;
  m_kd = t_d;
  m_pterm = 0;
  m_iterm = 0;
  m_dterm = 0;
  m_last_error = 0;
  m_error_guard = 20;
  m_target = t_target;
  m_lower_limit = static_cast<T>(0);
  m_upper_limit = static_cast<T>(0);
}

template<class T> T PID<T>::GetTarget() const { return m_target; }

template<class T> void PID<T>::SetTarget(const T t_target) { m_target = t_target; }

template<class T> double PID<T>::GetErrorGuard() const { return m_error_guard; }

template<class T> void PID<T>::SetErrorGuard(const double t_error_guard) { m_error_guard = t_error_guard; }

template<class T> std::pair<T, T> PID<T>::GetLimits() const { return std::pair<T, T>{ m_upper_limit, m_lower_limit }; }

template<class T> void PID<T>::SetLimits(const T t_lower_limit, const T t_upper_limit)
{
  m_lower_limit = t_lower_limit;
  m_upper_limit = t_upper_limit;
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

template<class T> void PID<T>::UpdateRaw(const T t_feedback, const double t_time_Delta)
{
  T error = m_target - t_feedback;
  T error_delta = error - m_last_error;
  m_last_error = error;
  m_pterm = m_kp * error;
  m_iterm += error * t_time_Delta;

  if (m_iterm < -m_error_guard)
    m_iterm = -m_error_guard;
  else if (m_iterm > m_error_guard)
    m_iterm = m_error_guard;

  m_dterm = error_delta / t_time_Delta;
  m_output = m_pterm + (m_ki * m_iterm) + (m_kd * m_dterm);
}

template<class T> void PID<T>::UpdateLimited(const T t_feedback, const double t_time_Delta)
{
  if (t_feedback < (-m_limit_difference)) {
    UpdateRaw(-m_limit_difference, t_time_Delta);
  } else if (t_feedback > (m_limit_difference)) {
    UpdateRaw(m_limit_difference, t_time_Delta);
  }
}
}// namespace TimeBox

#endif// PID_HPP