/** Memmove for LowRISC (copy tags when all arguments are aligned) */

#include "tag.h"

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


/* Copy forwards. len in words */
static inline void wordcopy_fwd_tagged(op_t* dest, op_t* src, size_t len) {
  /* FIXME consider unrolling, see string/wordcopy.c.
   * However, the compiler really ought to be able to do that nowadays! */
  for(;len;dest++,src++,len--) {
    op_t a = *src;
    char tag = load_tag(src);
    *dest = a;
    store_tag(dest, tag);
  }
}

/* Copy backwards. len in words */
static inline void wordcopy_bwd_tagged(op_t* dest, op_t* src, size_t len) {
  /* FIXME consider unrolling, see string/wordcopy.c.
   * However, the compiler really ought to be able to do that nowadays! */
  dest += (len-1);
  src += (len-1);
  for(;len;dest--,src--,len--) {
    op_t a = *src;
    char tag = load_tag(src);
    *dest = a;
    store_tag(dest, tag);
  }
}

/* Tag-copying version of memmove */

void* __riscv_memmove_tagged_longs(op_t* dest, op_t* src, size_t len) {
  unsigned long int dstp = (long int) dest;
  unsigned long int srcp = (long int) src;
  /* This test makes the forward copying code be used whenever possible.
     Reduces the working set.  */
  if (dstp - srcp >= len)       /* *Unsigned* compare!  */
    {
      /* Copy from the beginning to the end.  */

#if MEMCPY_OK_FOR_FWD_MEMMOVE
      /* Will include tags */
      dest = memcpy (dest, src, len);
#else
# if PAGE_COPY_THRESHOLD > 0
      // REDFLAG Untested, only used on Mach
      size_t lenBytes = len * sizeof(op_t);
      if(lenBytes > PAGE_COPY_THRESHOLD) {
        /* Copy whole pages from SRCP to DSTP by virtual address
           manipulation, as much as possible.  */
        PAGE_COPY_FWD_MAYBE (dstp, srcp, lenBytes, lenBytes);
        // Update len
        len = lenBytes / sizeof(op_t);
      }
# endif

      /* Copy words. We know it is fully aligned so there won't be any
       * bytes left over. */
      wordcopy_fwd_tagged(dest, src, len);
#endif /* MEMCPY_OK_FOR_FWD_MEMMOVE */
    } else {
      /* Copy words backwards. Fully aligned so no bytes left over. */
      wordcopy_bwd_tagged(dest, src, len);
    }
    return dest;
}

/* Choose the right version at runtime */

void* memmove(void *dst, const void* src, size_t length) {
  if(UNALIGNED3(dst, src, length))
    return __riscv_memmove_no_tags(dst, src, length);
  else
    return __riscv_memmove_tagged_longs((op_t*)dst, (op_t*)src, length / sizeof(op_t));
  return dst;
}

libc_hidden_builtin_def (memmove)
