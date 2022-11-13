#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#pragma once

// Includes here

template<typename T> class Singleton
{
public:
  static T &GetInstance() noexcept(std::is_nothrow_constructible<T>::value)
  {
    static T instance;
    return instance;
  }

protected:
  Singleton() noexcept = default;
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

  virtual ~Singleton() = default;
};

#endif// SINGLETON_HPP