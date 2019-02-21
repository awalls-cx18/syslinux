#ifndef _X86_CPU_H
#define _X86_CPU_H

#include <klibc/compiler.h>
#include <inttypes.h>
#include <stdbool.h>
#include <x86/regs.h>

static inline __constfunc unsigned long cpu_has_eflags(unsigned long flags)
{
    unsigned long f1, f2;

    if (__builtin_constant_p(flags)) {
	if (!(flags & ~(unsigned long)KNOWN_EFLAGS))
	    return flags;	/* We know we have them all */
    }

    asm("pushf ; "
	"pushf ; "
	"pop %0 ; "
	"mov %0,%1 ; "
	"xor %2,%1 ; "
	"push %1 ; "
	"popf ; "
	"pushf ; "
	"pop %1 ; "
	"popf"
	: "=&r" (f1), "=&r" (f2) : "ri" (flags));

    return (f1 ^ f2) & flags;
}

static inline __constfunc bool cpu_has_eflag(unsigned long flag)
{
    return cpu_has_eflags(flag) != 0;
}

static inline uint64_t rdtsc(void)
{
    uint32_t eax, edx;
    asm volatile("rdtsc" : "=a" (eax), "=d" (edx));
    return eax + ((uint64_t)edx << 32);
}

static inline uint32_t rdtscl(void)
{
    uint32_t v;
    asm volatile("rdtsc" : "=a" (v) : : "edx");
    return v;
}

static inline void cpuid_count(uint32_t op, uint32_t cnt,
			       uint32_t *eax, uint32_t *ebx,
			       uint32_t *ecx, uint32_t *edx)
{
#if defined(__i386__) && defined(__PIC__)
    asm volatile("movl %%ebx,%1 ; "
		 "cpuid ; "
		 "xchgl %1,%%ebx"
		 : "=a" (*eax), "=SD" (*ebx), "=c" (*ecx), "=d" (*edx)
		 : "a"(op), "c"(cnt));
#else
    asm volatile("cpuid"
		 : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
		 : "a"(op), "c"(cnt));
#endif
}

static inline void cpuid(uint32_t op,
			 uint32_t *eax, uint32_t *ebx,
			 uint32_t *ecx, uint32_t *edx)
{
    cpuid_count(op, 0, eax, ebx, ecx, edx);
}

static inline __constfunc uint32_t cpuid_eax(uint32_t level)
{
    uint32_t eax, ebx, ecx, edx;

    cpuid_count(level, 0, &eax, &ebx, &ecx, &edx);
    return eax;
}

static inline __constfunc uint32_t cpuid_ebx(uint32_t level)
{
    uint32_t eax, ebx, ecx, edx;

    cpuid_count(level, 0, &eax, &ebx, &ecx, &edx);
    return ebx;
}

static inline __constfunc uint32_t cpuid_ecx(uint32_t level)
{
    uint32_t eax, ebx, ecx, edx;

    cpuid_count(level, 0, &eax, &ebx, &ecx, &edx);
    return ecx;
}

static inline __constfunc uint32_t cpuid_edx(uint32_t level)
{
    uint32_t eax, ebx, ecx, edx;

    cpuid_count(level, 0, &eax, &ebx, &ecx, &edx);
    return edx;
}

static inline uint64_t rdmsr(uint32_t msr)
{
    uint32_t eax, edx;

    asm volatile("rdmsr" : "=a" (eax), "=d" (edx) : "c" (msr));
    return eax + ((uint64_t)edx << 32);
}

static inline void wrmsr(uint64_t v, uint32_t msr)
{
    uint32_t eax = v;
    uint32_t edx = v >> 32;

    asm volatile("wrmsr" : : "a" (eax), "d" (edx), "c" (msr));
}

static inline unsigned long long get_cr0(void)
{
    unsigned long v;
    asm volatile("mov %%cr0,%0" : "=r" (v));
    return v;
}

static inline void set_cr0(unsigned long v)
{
    asm volatile("mov %0,%%cr0" : : "r" (v));
}

static inline unsigned long get_cr4(void)
{
    unsigned long v;
    asm volatile("mov %%cr4,%0" : "=r" (v));
    return v;
}

static inline void set_cr4(unsigned long v)
{
    asm volatile("mov %0,%%cr4" : : "r" (v));
}

static inline uint32_t get_mxcsr(void)
{
    uint32_t v;
    asm("stmxcsr %0" : "=m" (v));
    return v;
}

static inline void set_mxcsr(uint32_t v)
{
    asm volatile("ldmxcsr %0" : : "m" (v));
}

static inline void fninit(void)
{
    asm volatile("fninit");
}

static inline void cpu_relax(void)
{
    asm volatile("rep ; nop");
}

static inline void hlt(void)
{
    asm volatile("hlt" : : : "memory");
}

static inline void cli(void)
{
    asm volatile("cli" : : : "memory");
}

static inline void sti(void)
{
    asm volatile("sti" : : : "memory");
}

static inline unsigned long get_eflags(void)
{
    unsigned long v;

    asm volatile("pushf ; pop %0" : "=rm" (v));
    return v;
}

static inline void set_eflags(unsigned long v)
{
    asm volatile("push %0 ; popf" : : "g" (v) : "memory");
}

typedef unsigned long irq_state_t;

static inline irq_state_t irq_state(void)
{
    return get_eflags();
}

static inline irq_state_t irq_save(void)
{
    irq_state_t v = irq_state();
    cli();
    return v;
}

static inline void irq_restore(irq_state_t v)
{
    set_eflags(v);
}

#endif /* _X86_CPU_H */
