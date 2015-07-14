#include <string.h>
#include <stdint.h>
#include "tag.h"

#define TAGGED_MEMORY

void* memcpy(void* aa, const void* bb, size_t n)
{
  #define BODY(a, b, t) { \
    t tt = *b; \
    a++, b++; \
    *(a-1) = tt; \
  }

#ifdef TAGGED_MEMORY
  #define BODY_TAGGED(a, b) { \
    long tt = *b; \
    char tag = load_tag(b); \
    a++, b++; \
    *(a-1) = tt; \
    store_tag(a-1, tag); \
  }
#endif

  char* a = (char*)aa;
  const char* b = (const char*)bb;
  char* end = a+n;
  uintptr_t msk = sizeof(long)-1;
#ifdef TAGGED_MEMORY
  int copyTags = !((uintptr_t)a & msk) && !((uintptr_t)b & msk) && !(n & msk);
#endif
  if (__builtin_expect(((uintptr_t)a & msk) != ((uintptr_t)b & msk) || n < sizeof(long), 0))
  {
small:
    if (__builtin_expect(a < end, 1))
      while (a < end)
        BODY(a, b, char);
    return aa;
  }

  if (__builtin_expect(((uintptr_t)a & msk) != 0, 0))
    while ((uintptr_t)a & msk)
      BODY(a, b, char);

  long* la = (long*)a;
  const long* lb = (const long*)b;
  long* lend = (long*)((uintptr_t)end & ~msk);

#ifdef TAGGED_MEMORY
  if(copyTags) {
    if(__builtin_expect(la < lend-3, 0)) {
      /* Only copy four at a time because will need regs for tags too.
       * Compiler will use a whole reg for a char, right? */
      while (la < lend-8)
      {
        long b0 = *lb++;
        char t0 = load_tag(lb-1);
        long b1 = *lb++;
        char t1 = load_tag(lb-1);
        long b2 = *lb++;
        char t2 = load_tag(lb-1);
        long b3 = *lb++;
        char t3 = load_tag(lb-1);
        *la++ = b0;
        store_tag(la-1, t0);
        *la++ = b1;
        store_tag(la-1, t1);
        *la++ = b2;
        store_tag(la-1, t2);
        *la++ = b3;
        store_tag(la-1, t3);
      }
    }
  } else {
#endif
    if(__builtin_expect(la < lend-8, 0)) {
      while (la < lend-8)
      {
        long b0 = *lb++;
        long b1 = *lb++;
        long b2 = *lb++;
        long b3 = *lb++;
        long b4 = *lb++;
        long b5 = *lb++;
        long b6 = *lb++;
        long b7 = *lb++;
        long b8 = *lb++;
        *la++ = b0;
        *la++ = b1;
        *la++ = b2;
        *la++ = b3;
        *la++ = b4;
        *la++ = b5;
        *la++ = b6;
        *la++ = b7;
        *la++ = b8;
      }
    }
#ifdef TAGGED_MEMORY
  }

  if(copyTags) {
    while (la < lend)
      BODY_TAGGED(la, lb);
  } else {
#endif
    while (la < lend)
      BODY(la, lb, long);
#ifdef TAGGED_MEMORY
  }
#endif

maybe_done:
  a = (char*)la;
  b = (const char*)lb;
  if (__builtin_expect(a < end, 0))
    goto small;
  return aa;
}
