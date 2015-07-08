#include <ieeefp.h>

<<<<<<< HEAD
#ifdef __riscv_hard_float
=======
>>>>>>> hx/master
static void fssr(int value)
{
  asm volatile ("fssr %0" :: "r"(value));
}

static int frsr()
{
  int value;
  asm volatile ("frsr %0" : "=r"(value));
  return value;
}
<<<<<<< HEAD
#endif
=======
>>>>>>> hx/master

fp_except fpgetmask(void)
{
  return 0;
}

fp_rnd fpgetround(void)
{
<<<<<<< HEAD
#ifdef __riscv_hard_float
  int rm = frsr() >> 5;
  return rm == 0 ? FP_RN : rm == 1 ? FP_RZ : rm == 2 ? FP_RM : FP_RP;
#else
  return FP_RZ;
#endif
=======
  int rm = frsr() >> 5;
  return rm == 0 ? FP_RN : rm == 1 ? FP_RZ : rm == 2 ? FP_RM : FP_RP;
>>>>>>> hx/master
}

fp_except fpgetsticky(void)
{
<<<<<<< HEAD
#ifdef __riscv_hard_float
  return frsr() & 0x1f;
#else
  return 0;
#endif
=======
  return frsr() & 0x1f;
>>>>>>> hx/master
}

fp_except fpsetmask(fp_except mask)
{
<<<<<<< HEAD
  return -1;
=======
  return 0;
>>>>>>> hx/master
}

fp_rnd fpsetround(fp_rnd rnd_dir)
{
<<<<<<< HEAD
#ifdef __riscv_hard_float
=======
>>>>>>> hx/master
  int fsr = frsr();
  int rm = fsr >> 5;
  int new_rm = rnd_dir == FP_RN ? 0 : rnd_dir == FP_RZ ? 1 : rnd_dir == FP_RM ? 2 : 3;
  fssr(new_rm << 5 | fsr & 0x1f);
  return rm == 0 ? FP_RN : rm == 1 ? FP_RZ : rm == 2 ? FP_RM : FP_RP;
<<<<<<< HEAD
#else
  return -1;
#endif
=======
>>>>>>> hx/master
}

fp_except fpsetsticky(fp_except sticky)
{
<<<<<<< HEAD
#ifdef __riscv_hard_float
  int fsr = frsr();
  fssr(sticky & 0x1f | fsr & ~0x1f);
  return fsr & 0x1f;
#else
  return -1;
#endif
=======
  int fsr = frsr();
  fssr(sticky & 0x1f | fsr & ~0x1f);
  return fsr & 0x1f;
>>>>>>> hx/master
}
