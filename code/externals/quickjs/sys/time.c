#include "time.h"
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <assert.h>
static VOID(WINAPI* fnGetSystemTimePreciseAsFileTime)(LPFILETIME) = NULL;


static double highResTimeInterval = 0;
static inline  void InitHighResRelativeTime() {
    LARGE_INTEGER perfFrequency;

    if (highResTimeInterval != 0)
        return;

    /* Retrieve high-resolution timer frequency
    * and precompute its reciprocal.
    */
    if (QueryPerformanceFrequency(&perfFrequency)) {
        highResTimeInterval = 1.0 / perfFrequency.QuadPart;
    }
    else {
        highResTimeInterval = -1;
    }

    assert(highResTimeInterval != 0);
}

static inline  void InitHighResAbsoluteTime() {
    FARPROC fp;
    HMODULE module;

    if (fnGetSystemTimePreciseAsFileTime != NULL)
        return;

    /* Use GetSystemTimeAsFileTime as fallbcak where GetSystemTimePreciseAsFileTime is not available */
    fnGetSystemTimePreciseAsFileTime = GetSystemTimeAsFileTime;
    module = GetModuleHandleA("kernel32.dll");
    if (module) {
        fp = GetProcAddress(module, "GetSystemTimePreciseAsFileTime");
        if (fp) {
            fnGetSystemTimePreciseAsFileTime = (VOID(WINAPI*)(LPFILETIME)) fp;
        }
    }

    assert(fnGetSystemTimePreciseAsFileTime != NULL);
}

void InitTimeFunctions() {
    InitHighResRelativeTime();
    InitHighResAbsoluteTime();
}

#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
int      gettimeofday(struct timeval* tv, struct timezone* tz)
{
    FILETIME ft;
    unsigned long long tmpres = 0;
    static int tzflag;

    if (NULL == fnGetSystemTimePreciseAsFileTime) {
        InitHighResAbsoluteTime();
    }

    if (NULL != tv) {
        fnGetSystemTimePreciseAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz) {
        if (!tzflag) {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

static const size_t kNsPerSec = 1000000000;
#define CLOCK_MONOTONIC 1
#define CLOCK_REALTIME 0
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
int clock_getres(clockid_t clock_id, struct timespec* res) {
    if (!res) {
        errno = EFAULT;
        return -1;
    }

    switch (clock_id) {
    case CLOCK_MONOTONIC: {
        LARGE_INTEGER freq;
        if (!QueryPerformanceFrequency(&freq)) {
            errno = EINVAL;
            return -1;
        }

        res->tv_sec = 0;
        res->tv_nsec = (long)((kNsPerSec + (freq.QuadPart >> 1)) / freq.QuadPart);
        if (res->tv_nsec < 1) {
            res->tv_nsec = 1;
        }

        return 0;
    }

    case CLOCK_REALTIME:
    case CLOCK_PROCESS_CPUTIME_ID:
    case CLOCK_THREAD_CPUTIME_ID: {
        DWORD adj, timeIncrement;
        BOOL adjDisabled;
        if (!GetSystemTimeAdjustment(&adj, &timeIncrement, &adjDisabled)) {
            errno = EINVAL;
            return -1;
        }

        res->tv_sec = 0;
        res->tv_nsec = timeIncrement * 100;
        return 0;
    }

    default:
        errno = EINVAL;
        return -1;
    }
}

inline unsigned long long ftToUint(FILETIME ft) {
    ULARGE_INTEGER i;
    i.HighPart = ft.dwHighDateTime;
    i.LowPart = ft.dwLowDateTime;
    return i.QuadPart;
};

inline const int timeToTimespec(struct timespec* tp, unsigned long long t) {
    const size_t k100NsPerSec = kNsPerSec / 100;

    // The filetimes t is based on are represented in
    // 100ns's. (ie. a value of 4 is 400ns)
    tp->tv_sec = t / k100NsPerSec;
    tp->tv_nsec = ((long)(t % k100NsPerSec)) * 100;
    return 0;
};
int clock_gettime(clockid_t clock_id, struct timespec* tp) {
    if (!tp) {
        errno = EFAULT;
        return -1;
    }


    FILETIME createTime, exitTime, kernalTime, userTime;
    switch (clock_id) {
    case CLOCK_REALTIME: {
        const unsigned long long kDeltaEpochIn100NS = 116444736000000000ULL;

        GetSystemTimeAsFileTime(&createTime);
        return timeToTimespec(tp, ftToUint(createTime) - kDeltaEpochIn100NS);
    }
    case CLOCK_PROCESS_CPUTIME_ID: {
        if (!GetProcessTimes(
            GetCurrentProcess(),
            &createTime,
            &exitTime,
            &kernalTime,
            &userTime)) {
            errno = EINVAL;
            return -1;
        }

        return timeToTimespec(tp, ftToUint(kernalTime) + ftToUint(userTime));
    }
    case CLOCK_THREAD_CPUTIME_ID: {
        if (!GetThreadTimes(
            GetCurrentThread(),
            &createTime,
            &exitTime,
            &kernalTime,
            &userTime)) {
            errno = EINVAL;
            return -1;
        }

        return timeToTimespec(tp, ftToUint(kernalTime) + ftToUint(userTime));
    }
    case CLOCK_MONOTONIC: {
        LARGE_INTEGER fl, cl;
        if (!QueryPerformanceFrequency(&fl) || !QueryPerformanceCounter(&cl)) {
            errno = EINVAL;
            return -1;
        }

        long long freq = fl.QuadPart;
        long long counter = cl.QuadPart;
        tp->tv_sec = counter / freq;
        tp->tv_nsec = (long)(((counter % freq) * kNsPerSec + (freq >> 1)) / freq);
        if (tp->tv_nsec >= kNsPerSec) {
            tp->tv_sec++;
            tp->tv_nsec -= kNsPerSec;
        }

        return 0;
    }

    default:
        errno = EINVAL;
        return -1;
    }
}