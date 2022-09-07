#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#pragma once

#include <iomanip>
#include <cassert>

unsigned SetCpuMask(int t_mask = 0)
{
  int rc{ 0 };
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(t_mask, &mask);
  unsigned number_of_cpus{ CPU_COUNT(&mask) };
  assert((rc = sched_setaffinity(0, sizeof(mask), &mask)) == 0);

  return number_of_cpus;
}

template<typename T> T ComputeVectorAverage(std::vector<T> data)
{
  T sum{ 0.0 };
  for (std::size_t i = 0; i < data.size(); ++i) { sum += data.at(i); }
  return sum / data.size();
}

#endif// UTILITIES_HPP