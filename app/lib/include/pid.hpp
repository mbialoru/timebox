#ifndef PID_HPP
#define PID_HPP

#pragma once

#include <boost/log/trivial.hpp>
#include <cmath>
#include <tuple>

namespace TimeBox {

template<typename T> class PID
{
public:
  PID(double, double, double, T) noexcept(std::is_arithmetic<T>::value);
  ~PID() = default;

  void update_raw(T, double);
  void update_limited(T, double);
  void set_terms(double, double, double);
  void set_target(T);
  void set_limits(T, T);
  void set_error_guard(double);

  T get_target() const;
  T get_output_raw() const;
  T get_output_limited() const;

  std::tuple<double, double, double> get_terms() const;

  std::pair<T, T> get_limits() const;

  double get_error_guard() const;

private:
  T m_target;
  T m_output;
  T m_last_error;
  T m_lower_limit;
  T m_upper_limit;
  T m_middle_limit;
  T m_limit_difference;

  double m_kp;
  double m_ki;
  double m_kd;
  double m_error_guard{ 0 };
  double m_p_term{ 0 };
  double m_i_term{ 0 };
  double m_d_term{ 0 };
};

template<typename T>
PID<T>::PID(const double t_p, const double t_i, const double t_d, const T t_target) noexcept(
  std::is_arithmetic<T>::value)
  : m_target(t_target), m_kp(t_p), m_ki(t_i), m_kd(t_d)
{
  m_error_guard = 20.0;
  m_output = static_cast<T>(0);
  m_last_error = static_cast<T>(0);
  m_lower_limit = static_cast<T>(0);
  m_upper_limit = static_cast<T>(0);
  m_middle_limit = static_cast<T>(0);
  m_limit_difference = static_cast<T>(0);
}

template<typename T> void PID<T>::update_raw(const T t_feedback, const double t_time_delta)
{
  T error = m_target - t_feedback;
  T error_delta = error - m_last_error;
  m_last_error = error;
  m_p_term = m_kp * error;
  m_i_term += error * t_time_delta;

  if (m_i_term < -m_error_guard)
    m_i_term = -m_error_guard;
  else if (m_i_term > m_error_guard)
    m_i_term = m_error_guard;

  // NOTE: prevent creation of NaN with division
  if (t_time_delta != 0 and std::isfinite(t_time_delta) and std::isfinite(error_delta)) {
    m_d_term = error_delta / t_time_delta;
  }

  m_output = m_p_term + (m_ki * m_i_term) + (m_kd * m_d_term);
}

template<typename T> void PID<T>::update_limited(const T t_feedback, const double t_time_delta)
{
  if (t_feedback < (-m_limit_difference)) {
    update_raw(-m_limit_difference, t_time_delta);
  } else if (t_feedback > (m_limit_difference)) {
    update_raw(m_limit_difference, t_time_delta);
  } else {
    update_raw(t_feedback, t_time_delta);
  }
}

template<typename T> void PID<T>::set_terms(const double t_p, const double t_i, const double t_d)
{
  m_kp = t_p;
  m_ki = t_i;
  m_kd = t_d;
}

template<typename T> void PID<T>::set_target(const T t_target) { m_target = t_target; }

template<typename T> void PID<T>::set_limits(const T t_lower_limit, const T t_upper_limit)
{
  m_lower_limit = t_lower_limit;
  m_upper_limit = t_upper_limit;
  m_limit_difference = (m_upper_limit - m_lower_limit) / 2;
  m_middle_limit = m_lower_limit + m_limit_difference;
}

template<typename T> void PID<T>::set_error_guard(const double t_error_guard) { m_error_guard = t_error_guard; }

template<typename T> T PID<T>::get_target() const { return m_target; }

template<typename T> T PID<T>::get_output_raw() const { return m_output; }

template<typename T> T PID<T>::get_output_limited() const
{
  if (m_upper_limit == static_cast<T>(0) && m_lower_limit == static_cast<T>(0)) {
    BOOST_LOG_TRIVIAL(warning) << "Using limited PID output when limits are not set";
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

template<typename T> std::tuple<double, double, double> PID<T>::get_terms() const
{
  return std::make_tuple(m_kp, m_ki, m_kd);
}

template<typename T> std::pair<T, T> PID<T>::get_limits() const
{
  return std::pair<T, T>{ m_lower_limit, m_upper_limit };
}

template<typename T> double PID<T>::get_error_guard() const { return m_error_guard; }

}// namespace TimeBox

#endif// PID_HPP