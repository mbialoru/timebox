#include "defines.hpp"

/* Description
==============
Prototype to practice and explore systemd developement libraries
==============
*/

/* Notes
==============

==============
*/

#if PROTO_SYSTEMD

#include <gtest/gtest.h>
#include <iostream>
#include <stdlib.h>

bool TimeSyncServiceIsRunning()
{
  char line[100];

  // Solution 1 ?
  FILE* f = popen("pidof systemd-timesyncd", "r");
  fgets(line, 100, f);
  pid_t pid = strtoul(line, NULL, 10);

  std::cout << pid;

  pclose(f);

  return false;
}

TEST(Pidof_testing, try_to_get_pidof)
{
  TimeSyncServiceIsRunning();
}

#endif