#include "defines.hpp"
#include "utils.hpp"

/* Description
==============
Prototype to practice and explore usage of adjtimex to control pacing of system
clock
==============
*/

/* Notes
==============
Tests which require sudo might fail when launched from within devcontainer. We
are trying to actively change kernel parameters and it's not possible to modify
host machine like this from container! To run these tests, a baremetal or VM is
advised - and do not forget about privileges.
==============
*/

#if PROTO_ADJTIMEX

#include <gtest/gtest.h>
#include <sys/timex.h>
#include <iostream>
#include <errno.h>

// Note about used units:
/*
  In struct timex, freq, ppsfreq, and stabil are ppm (parts per
  million) with a 16-bit fractional part, which means that a value
  of 1 in one of those fields actually means 2^-16 ppm, and
  2^16=65536 is 1 ppm.  This is the case for both input values (in
  the case of freq) and output values.
*/

// Note about tests failing sometimes, from docs:
/*
  [...]
  starting with Linux 3.4, the call operates asynchronously and
  the return value usually will not reflect a state change caused
  by the call itself.
*/

void debug_print_timex(timex& t)
{
  std::cout <<
    "modes: " << t.modes << std::endl <<
    "offset: " << t.offset << std::endl <<
    "freq: " << t.freq << std::endl <<
    "maxerror: " << t.maxerror << std::endl <<
    "esterror: " << t.esterror << std::endl <<
    "status: " << t.status << std::endl <<
    "constant: " << t.constant << std::endl <<
    "precision: " << t.precision << std::endl <<
    "tolerance: " << t.tolerance << std::endl <<
    "timeval_sec: " << t.time.tv_sec << std::endl <<
    "timeval_usec: " << t.time.tv_usec << std::endl <<
    "tick: " << t.tick << std::endl <<
    "ppsfreq: " << t.ppsfreq << std::endl <<
    "jitter: " << t.jitter << std::endl <<
    "shift: " << t.shift << std::endl <<
    "stabil: " << t.stabil << std::endl <<
    "calcnt: " << t.calcnt << std::endl <<
    "errcnt: " << t.errcnt << std::endl <<
    "stbcnt: " << t.stbcnt << std::endl <<
    "tai: " << t.tai << std::endl;
}

TEST(AdjtimexTest, timex_from_adjtimex)
{
  GTEST_SKIP();
  timex buf;
  int res{ adjtimex(&buf) };

  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);
}

TEST(AdjtimexTest, timex_from_ntp_adjtime)
{
  GTEST_SKIP();
  timex buf;
  int res{ ntp_adjtime(&buf) };

  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);
}

TEST(AdjtimexTest, print_timex_structure)
{
  GTEST_SKIP() << "Only for testing debug tools";
  timex buf;
  int res{ adjtimex(&buf) };
  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);
  debug_print_timex(buf);
}


TEST(AdjtimexTest, change_tick_value)
{
#if SUDO_NODOCKER
  if (!RunningAsRoot())
    GTEST_FAIL() << "MUST RUN AS ROOT";
  if (RunningFromDockerContainer())
    GTEST_FAIL() << "CANNOT RUN FROM DOCKER";
#else
  GTEST_SKIP() << "NEEDS ROOT PRIVILEGES AND CANNOT RUN FROM DOCKER CONTAINER"
#endif

  timex cur;
  cur.modes = 0;
  int res{ adjtimex(&cur) };
  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);
  timex buf = cur;

  buf.modes |= ADJ_TICK;
  buf.tick = 9500;
  res = adjtimex(&buf);
  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);

  timex tmp;
  res = adjtimex(&tmp);
  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);
  EXPECT_EQ(tmp.tick, 9000);

  // Revert the change
  res = adjtimex(&cur);
  EXPECT_EQ(res, TIME_OK);
  EXPECT_EQ(errno, 0);
}

TEST(AdjtimexTest, change_tick_value_invalid)
{
#if SUDO_NODOCKER
  if (!RunningAsRoot())
    GTEST_FAIL() << "MUST RUN AS ROOT";
  if (RunningFromDockerContainer())
    GTEST_FAIL() << "CANNOT RUN FROM DOCKER";
#else
  GTEST_SKIP() << "NEEDS ROOT PRIVILEGES AND CANNOT RUN FROM DOCKER CONTAINER"
#endif

  timex cur;
  int res{ adjtimex(&cur) };
  EXPECT_EQ(res, 0);
  EXPECT_EQ(errno, 0);

  timex buf = cur;

  buf.tick = 8999;
  res = adjtimex(&buf);
  EXPECT_EQ(res, -1);
  EXPECT_EQ(errno, EINVAL);
}

#endif