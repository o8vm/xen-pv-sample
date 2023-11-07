#include<stdint.h>
#include"/usr/lib/gcc/x86_64-redhat-linux/4.8.2/include/stddef.h"
#include<xen/xen.h>
#include"time.h"
#include"hypercall-x86_32.h"

char stack[8192];

extern char shared_info[4096];
extern shared_info_t* HYPERVISOR_shared_info;

// start_info
union start_info_union
{
    start_info_t start_info;
    char padding[512];
};
union start_info_union start_info_union;
#define start_info (start_info_union.start_info)


// Helpers to map shared_info
static shared_info_t *map_shared_info(void *p)
{
    int rc;
    start_info_t *si = p;
    unsigned long pa = si->shared_info;

    if ((rc = HYPERVISOR_update_va_mapping(
        (unsigned long)shared_info, __pte(pa | 7),  UVMF_INVLPG)) )
    {
        HYPERVISOR_console_io(CONSOLEIO_write, 28, "Failed to map shared_info!!\n");
        return (shared_info_t *)0;
    }
    return (shared_info_t *)shared_info;
}

void *memcopy(void *dest, const void *src, size_t count)
{
    char *tmp = (char *)dest;
    const char *s = src;
    while (count--)
        *tmp++ = *s--;
    return dest;
}

void start_kernel(start_info_t *si)
{
    struct timeval tp;
    static char buf[] = "0000000000000000000\n";
    memcopy(&start_info, si, sizeof(start_info));
    HYPERVISOR_console_io(CONSOLEIO_write, 14, "\nKernel Start\n");
    HYPERVISOR_shared_info = map_shared_info(si);

    if (gettimeofday(&tp, 0) != 0) {
        HYPERVISOR_console_io(CONSOLEIO_write, 15,"#: gtod FAILED\n");
    } else {
        convert(tp.tv_sec, buf);
        HYPERVISOR_console_io(CONSOLEIO_write,20,buf);
    }
    HYPERVISOR_console_io(CONSOLEIO_write,5,"End.\n");
    while(1);
}
