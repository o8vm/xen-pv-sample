typedef long time_t;
typedef long suseconds_t;

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

struct timezone {
};

struct timeval {
    time_t      tv_sec;     // seconds
    suseconds_t tv_usec;    // microseconds
};

int gettimeofday(struct timeval *tv, void *tz);
void convert(time_t t, char buf[]);