/** Memmove for LowRISC (copy tags when all arguments are aligned) */

/* FIXME should be defined by gcc if applicable */
#define TAGGED_MEMORY

#ifndef TAGGED_MEMORY
#include "string/memmove.c"
#else

#include "sys/platform/tag.h"

/* Build the no-tag-copying version of memmove */

#include <stddef.h>
#include <string.h>

void* __riscv_memmove_no_tags(void* dst, const void* src, size_t len);

/* Persuade memmove.c not to redefine memmove etc */
#define MEMMOVE __riscv_memmove_no_tags
#define a1 dest
#define a2 src
#define a1const
#define a2const const
#define memmove

#include "string/memmove.c"

#undef memmove

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED3(X, Y, Z) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)) | \
  ((long)Z & (sizeof (long) - 1)))

/* All arguments must be long-aligned, and the length is in long's. */
unsigned long* __riscv_memmove_tagged_longs(unsigned long *dst, 
  const unsigned long* src, size_t length) {

#define TAG_COPY_BACKWARD(d, s, length) do { \
  d += length; \
  s += length; \
  s--; d--; \
  for(;length--;s--,d--) { \
    unsigned long l = *s; \
    char tag = __riscv_load_tag(s); \
    *d = l; \
    __riscv_store_tag(d, tag); \
  } \
} while(0);

#define TAG_COPY_FORWARD(d, s, length) do { \
  for(;length--;s++,d++) { \
    unsigned long l = *s; \
    char tag = __riscv_load_tag(s); \
    *d = l; \
    __riscv_store_tag(d, tag); \
  } \
} while(0)

  unsigned long * const ret = dst;

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)

  if (src < dst && dst < src + length)
    {
      TAG_COPY_BACKWARD(dst, src, length);
      return ret;
    }
  else
    {
      TAG_COPY_FORWARD(dst, src, length);
      return ret;
    }
#else
  if (src < dst && dst < src + length)
    {
      TAG_COPY_BACKWARD(dst, src, length);
      return ret;
    }
  else
    {
      /* Use optimizing algorithm for a non-destructive copy to closely 
         match memcpy. */

      /* Copy 4X long words at a time if possible.  */
      while (length >= 4)
        {
#define COPY(aligned_dst, aligned_src) do { \
  unsigned long val = *aligned_src; \
  char tag = __riscv_load_tag(aligned_src); \
  *aligned_dst = val; \
  __riscv_store_tag(aligned_dst, tag); \
  aligned_dst++; \
  aligned_src++; \
} while(0);

          COPY(dst, src);
          COPY(dst, src);
          COPY(dst, src);
          COPY(dst, src);
          length -= 4;
        }

      /* Copy one long word at a time if possible.  */
      while (length >= 1)
        {
          COPY(dst, src)
          length--;
        }
    }

  return ret;
#endif /* not PREFER_SIZE_OVER_SPEED */
}

void* memmove(void *dst, const void* src, size_t length) {
  if(UNALIGNED3(dst, src, length))
    return __riscv_memmove_no_tags(dst, src, length);
  else
    return __riscv_memmove_tagged_longs((unsigned long*)dst, 
      (const unsigned long*)src, length/sizeof(unsigned long));
}

libc_hidden_builtin_def (memmove)

#endif /* TAGGED_MEMORY */
