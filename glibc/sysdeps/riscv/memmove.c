/** Memmove for LowRISC (copy tags when all arguments are aligned) */

/* Build the no-tag-copying version of memmove */

#define MEMMOVE __memmove_no_tags
#include "string/memmove.c"
#include "tag.h"

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

void* __memmove_with_tags(op_t* dest, op_t* src, size_t len) {
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
      /* Copy whole pages from SRCP to DSTP by virtual address
         manipulation, as much as possible.  */

      PAGE_COPY_FWD_MAYBE (dstp, srcp, len, len);

      /* Copy words. We know it is fully aligned so there won't be any
       * bytes left over. */
      wordcopy_fwd_tagged(dest, src, len / OPSIZ);
#endif /* MEMCPY_OK_FOR_FWD_MEMMOVE */
    } else {
      /* Copy words backwards. Fully aligned so no bytes left over. */
      wordcopy_bwd_tagged(dest, src, len / OPSIZ);
    }
    return dest;
}

/* Choose the right version at runtime */

void* memmove (dest, src, len)
     void *dest;
     const void *src;
     size_t len;
{
  if(!(((unsigned long)dest) % OPSIZ) && 
     !(((unsigned long)src) % OPSIZ) && !((len % OPSIZ))) {
    return __memmove_with_tags((op_t*)dest, (op_t*)src, len);
  } else {
    return __memmove_no_tags(a1, a2, len);
  }
}
