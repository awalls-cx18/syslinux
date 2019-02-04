/*
 * x86_64/math/init_fpu.c
 *
 * Enable the x86-64 FPU if it isn't already
 * The x86-64 baseline includes SSE2
 */

#include <inttypes.h>
#include <sys/fpu.h>
#include <x86/cpu.h>
#include <x86/regs.h>

int init_fpu(void)
{
    uint64_t cr0, cr4;

    cr0 = get_cr0();
    cr0 &= ~(CR0_EM | CR0_TS);
    cr0 |= CR0_MP;
    set_cr0(cr0);

    cr4 = get_cr4();
    cr4 |= CR4_OSFXSR;
    set_cr4(cr4);

    set_mxcsr(0x3f << 7);    	/* Mask all exceptions */
    
    fninit();			/* x87 unit initialization */
    
    return 0;
}
