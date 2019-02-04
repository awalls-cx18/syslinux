/*
 * i386/math/has_fpu.c
 *
 * Test for an x87 FPU, and do any necessary setup.
 */

#include <inttypes.h>
#include <sys/fpu.h>
#include <x86/cpu.h>
#include <x86/regs.h>

int init_fpu(void)
{
    uint32_t cr0;
    uint16_t fsw = 0xffff;
    uint16_t fcw = 0xf3ff;

    cr0 = get_cr0();
    cr0 &= ~(CR0_EM | CR0_TS);
    cr0 |= CR0_MP;
    set_cr0(cr0);

    fninit();

    asm volatile ("fnstsw %0" : "+m" (fsw));
    if (fsw != 0)
	return -1;

    asm volatile ("fnstcw %0" : "+m" (fcw));
    if ((fcw & 0x103f) != 0x3f)
	return -1;

    /* Techically, this could be a 386 with a 287.  We could add a check
       for that here... */

    return 0;
}
