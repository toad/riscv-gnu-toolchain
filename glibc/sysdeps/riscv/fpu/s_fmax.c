#include <math.h>

double __fmax (double x, double y)
{
<<<<<<< HEAD
#ifdef __riscv_soft_float
  if (isnan(x))
    return y;
  if (isnan(y))
    return x;
  return (x > y) ? x : y;
#else
  double res;
  asm ("fmax.d %0, %1, %2" : "=f"(res) : "f"(x), "f"(y));
  return res;
#endif
=======
  double res;
  asm ("fmax.d %0, %1, %2" : "=f"(res) : "f"(x), "f"(y));
  return res;
>>>>>>> hx/master
}
weak_alias (__fmax, fmax)
