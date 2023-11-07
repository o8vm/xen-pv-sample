#include<stdint.h>
#include<xen/xen.h>

#define __STR(x) #x
#define STR(x) __STR(x)

shared_info_t *HYPERVISOR_shared_info;

typedef struct { unsigned long pte_low, pte_high; } pte_t;

#define __pte(x) ({ unsigned long long _x = (x);        \
    ((pte_t) {(unsigned long)(_x), (unsigned long)(_x>>32)}); })


#define _hypercall3(type, name, a1, a2, a3) \
({\
    long __res, __ign1, __ign2, __ign3; \
    __asm volatile (\
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1), "=c" (__ign2), "=d" (__ign3) \
        : "1" ((long)(a1)), "2" ((long)(a2)), "3" ((long)(a3)) \
        : "memory" \
    );\
    (type)__res;\
})

#define _hypercall4(type, name, a1, a2, a3, a4) \
({ \
    long __res, __ign1, __ign2, __ign3, __ign4;		\
    __asm volatile ( \
        "call hypercall_page + ("STR(__HYPERVISOR_##name)" * 32)"\
        : "=a" (__res), "=b" (__ign1), "=c" (__ign2), "=d" (__ign3), "=S" (__ign4) \
        : "1" ((long)(a1)), "2" ((long)(a2)), "3" ((long)(a3)), "4" ((long)(a4)) \
        : "memory" ); \
    (type)__res; \
})

static inline int
HYPERVISOR_console_io(int cmd, int count, char *str)
{
    return _hypercall3(int, console_io, cmd, count, str);
}

static inline int
HYPERVISOR_update_va_mapping(unsigned long va, pte_t new_val, unsigned long flags)
{
    return _hypercall4(int, update_va_mapping, va, new_val.pte_low, new_val.pte_high, flags);
}