#include <errno.h>
#include <math.h>
#include "fpu_control.h"

double __fdim (double x, double y)
{
  double diff = x - y;
  
  if (x <= y)
    return 0.0;

<<<<<<< HEAD
#ifdef __riscv_soft_float
  if (isinf(diff))
    errno = ERANGE;
#else
  if (__builtin_expect(_FCLASS(diff) & _FCLASS_INF, 0))
    errno = ERANGE;
#endif
=======
  if (__builtin_expect(_FCLASS(diff) & _FCLASS_INF, 0))
    errno = ERANGE;
>>>>>>> hx/master

  return diff;
}
weak_alias (__fdim, fdim)
