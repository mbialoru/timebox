#ifndef UTILITIES_COMMON_HPP
#define UTILITIES_COMMON_HPP

#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <limits>
#include <map>
#include <regex>

#include "exceptions.hpp"

namespace TimeBox {

extern std::vector<std::size_t> baud_rate_list;

typedef std::pair<std::string, std::chrono::system_clock::time_point> TimeboxReadout;

const std::string correct_serial_readout_regex{ "[0-9]{1,2}\\:[0-9]{1,2}\\:[0-9]{1,2}\\.[0-9]{1,2}\\n?" };

template<typename T, typename U>
bool nearly_equal(T t_first, U t_second, std::size_t t_n_epsilon = 1) noexcept(
  std::is_floating_point<T>::value and std::is_floating_point<U>::value)
{
  T min_first{ t_first - (t_first - std::nextafter(t_first, std::numeric_limits<T>::lowest())) * t_n_epsilon };
  T max_first{ t_first + (std::nextafter(t_first, std::numeric_limits<T>::max()) - t_first) * t_n_epsilon };

  return min_first <= t_second and max_first >= t_second;
};

bool check_admin_privileges();
bool check_if_using_docker();
bool check_ntp_status();

std::chrono::system_clock::time_point string_to_timepoint(std::string);

std::string render_ascii_codes(const std::string &);
std::string render_nonprintable_characters(const std::string &);
std::string timepoint_to_string(std::chrono::system_clock::time_point);

std::vector<std::string> get_serial_devices_list();

void pause_ntp_service();
void start_ntp_service();

}// namespace TimeBox

#endif// UTILITIES_COMMON_HPP