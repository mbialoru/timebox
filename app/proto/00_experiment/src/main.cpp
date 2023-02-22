/* Description
==============
Prototype as a ground for experimenting
==============
*/

#include <gtest/gtest.h>
#include <iostream>

class A
{
private:
  int a;

public:
  A() { std::cout << "Called delegated constructor"; }

  A(int a) : A()
  {
    this->a = a;
    std::cout << "Finished overloaded constructor call";
  }
};

TEST(Proto_Experiment, empty)
{
  A a;
  A a2(5);
}
