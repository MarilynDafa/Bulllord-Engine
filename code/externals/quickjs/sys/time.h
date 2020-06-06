#include <stdlib.h>
#include <time.h>
#if !defined(_SSIZE_T_) && !defined(_SSIZE_T_DEFINED)
#ifdef _WIN64
typedef long long          ssize_t;
#else
typedef long              ssize_t;
#endif
# define SSIZE_MAX INTPTR_MAX
# define _SSIZE_T_
# define _SSIZE_T_DEFINED
#endif
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

#include <WinSock2.h>


typedef unsigned char clockid_t;
void     InitTimeFunctions();
extern int      gettimeofday(struct timeval* tv, struct timezone* tz);
extern int clock_getres(clockid_t clock_id, struct timespec* res);
extern int clock_gettime(clockid_t clock_id, struct timespec* tp);

#ifdef INT64_MAX
#undef INT64_MAX
#define INT64_MAX 9223372036854775807LL
#endif