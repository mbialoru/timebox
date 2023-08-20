#include <assert.h>
#include <errno.h>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

// #define MODE CLOCK_REALTIME
// #define MODE CLOCK_MONOTONIC
// #define MODE CLOCK_PROCESS_CPUTIME_ID
// #define MODE CLOCK_THREAD_CPUTIME_ID

int main(int argc, char **argv)
{
  int NumberOf = 1000;
  int Mode = 0;
  int Verbose = 0;
  int c;
  // l loops, m mode, h help, v verbose, k masK


  int rc;
  cpu_set_t mask;
  int doMaskOperation = 0;

  while ((c = getopt(argc, argv, "l:m:hkv")) != -1) {
    switch (c) {
    case 'l':// ell not one
      NumberOf = atoi(optarg);
      break;
    case 'm':
      Mode = atoi(optarg);
      break;
    case 'h':
      cout << "Usage: <command> -l <int> -m <mode>" << endl
           << "where -l represents the number of loops and "
           << "-m represents the mode 0..3 inclusive" << endl
           << "0 is CLOCK_REALTIME" << endl
           << "1 CLOCK_MONOTONIC" << endl
           << "2 CLOCK_PROCESS_CPUTIME_ID" << endl
           << "3 CLOCK_THREAD_CPUTIME_ID" << endl;
      break;
    case 'v':
      Verbose = 1;
      break;
    case 'k':// masK - sorry! Already using 'm'...
      doMaskOperation = 1;
      break;
    case '?':
      cerr << "XXX unimplemented! Sorry..." << endl;
      break;
    default:
      abort();
    }
  }

  if (doMaskOperation) {
    if (Verbose) { cout << "Setting CPU mask to CPU 0 only!" << endl; }
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    assert((rc = sched_setaffinity(0, sizeof(mask), &mask)) == 0);
  }

  if (Verbose) { cout << "Verbose: Mode in use: " << Mode << endl; }

  if (Verbose) {
    rc = sched_getaffinity(0, sizeof(mask), &mask);
    // cout << "getaffinity rc is " << rc << endl;
    // cout << "getaffinity mask is " << mask << endl;
    int numOfCPUs = CPU_COUNT(&mask);
    cout << "Number of CPU's is " << numOfCPUs << endl;
    for (int i = 0; i < sizeof(mask); ++i)// sizeof(mask) is 128 RW 21 Nov 2014
    {
      if (CPU_ISSET(i, &mask)) { cout << "CPU " << i << " is set" << endl; }
      // cout << "CPU " << i
      //      << " is " << (CPU_ISSET(i,&mask) ? "set " : "not set ") << endl;
    }
  }

  clockid_t cpuClockID;
  int err = clock_getcpuclockid(0, &cpuClockID);
  if (Verbose) {
    cout << "Verbose: clock_getcpuclockid(0) returned err " << err << endl;
    cout << "Verbose: clock_getcpuclockid(0) returned cpuClockID " << cpuClockID << endl;
  }

  timespec timeNumber[NumberOf];
  for (int i = 0; i < NumberOf; ++i) {
    err = clock_gettime(Mode, &timeNumber[i]);
    if (err != 0) {
      int errSave = errno;
      cerr << "errno is " << errSave << " NumberOf is " << NumberOf << endl;
      cerr << strerror(errSave) << endl;
      cerr << "Aborting due to this error" << endl;
      abort();
    }
  }

  for (int i = 0; i < NumberOf - 1; ++i) {
    cout << timeNumber[i + 1].tv_sec - timeNumber[i].tv_sec
              + (timeNumber[i + 1].tv_nsec - timeNumber[i].tv_nsec) / 1000000000.
         << endl;
  }
  return 0;
}
