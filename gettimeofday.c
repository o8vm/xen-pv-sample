#include <stdint.h>
#include <xen/xen.h>
#include "time.h"
#include "hypercall-x86_32.h"
#include "printf.h"

#define NANOSECONDS(tsc, shared_info) (tsc << shared_info->vcpu_info[0].time.tsc_shift) \
    * shared_info->vcpu_info[0].time.tsc_to_system_mul

#define RDTSC(x) __asm volatile ("rdtsc" : "=A"(tsc))

int gettimeofday(struct timeval *tp, void *tzp)
{
    uint64_t tsc;
    uint32_t version, wc_version;
    uint32_t seconds, nanoseconds, system_time;
    uint64_t old_tsc;
    shared_info_t *s = HYPERVISOR_shared_info;

    do
    {
        do
        {
            wc_version = s->wc_version;
            version = s->vcpu_info[0].time.version;
        } while ( (version & 1) == 1 || (wc_version & 1) == 1);
        seconds = s->wc_sec;
        nanoseconds = s->wc_nsec;
        system_time = s->vcpu_info[0].time.system_time;
        old_tsc = s->vcpu_info[0].time.tsc_timestamp;
    } while ( version != s->vcpu_info[0].time.version || wc_version != s->wc_version );

    RDTSC(tsc);
    tsc -= old_tsc;
    system_time += NANOSECONDS(tsc, s);
    nanoseconds += system_time;
    seconds += nanoseconds / 1000000000;
    nanoseconds = nanoseconds % 1000000000;
    tp->tv_sec = seconds;
    tp->tv_usec = nanoseconds * 1000;

    return 0;
}

void convert(time_t t, char buf[20])
{
    int a,b,c,d,e,f;
    int sec, min, hou, day, mon, yea;
    sec = t % 60;
    t /= 60;
    min = t % 60;
    t /= 60;
    hou = t % 24;
    t /= 24;
    //Convert Unix time to date
    a = (uint32_t) ((4 * t + 102032) / 146097 + 15);
    b = (uint32_t) (t + 2442113 + a - (a / 4));
    c = (20 * b - 2442) / 7305;
    d = b - 365 * c - (c / 4);
    e = d * 1000 / 30601;
    f = d - e * 30 - e * 601 / 1000;

    //January and February are counted as months 13 and 14 of the previous year
    if(e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }
    yea = c;
    mon = e;
    day = f;
    sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d\n", yea, mon, day, hou, min, sec);
}