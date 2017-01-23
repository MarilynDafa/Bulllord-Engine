#ifndef MM_ACCEL_H
#define MM_ACCEL_H

/* generic accelerations */
#define MM_ACCEL_DJBFFT		0x00000001

/* x86 accelerations */
#define MM_ACCEL_X86_MMX	0x80000000
#define MM_ACCEL_X86_3DNOW	0x40000000
#define MM_ACCEL_X86_MMXEXT	0x20000000

uint32_t mm_accel (void);

#endif /* MM_ACCEL_H */
