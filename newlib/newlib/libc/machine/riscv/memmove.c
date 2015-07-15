#include <string.h>
#include <_ansi.h>
#include <stddef.h>
#include <limits.h>

#define TAGGED_MEMORY

#ifdef TAGGED_MEMORY
#include "tag.h"
#endif

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED2(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED3(X, Y, Z) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)) | \
  ((long)Z & (sizeof (long) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIGBLOCKSIZE    (sizeof (long) << 2)

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLEBLOCKSIZE (sizeof (long))

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void* memmove(void *dst_void, const void* src_void, size_t length) {
#ifdef TAGGED_MEMORY
  int copyTags = !UNALIGNED3(src_void, dst_void, length);

#define TAG_COPY_BACKWARD(dst, src, length) do { \
  length /= sizeof(long); \
  long *d = (long*) dst; \
  long *s = (long*) src; \
  d += length; \
  s += length; \
  s--; d--; \
  for(;length--;s--,d--) { \
    long l = *s; \
    char tag = load_tag(s); \
    *d = l; \
    store_tag(d, tag); \
  } \
} while(0);

#define TAG_COPY_FORWARD(dst, src, length) do { \
  length /= sizeof(long); \
  long *d = (long*) dst; \
  long *s = (long*) src; \
  for(;length--;s++,d++) { \
    long l = *s; \
    char tag = load_tag(s); \
    *d = l; \
    store_tag(d, tag); \
  } \
} while(0)

#endif /* TAGGED_MEMORY */

#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)

  char *dst = dst_void;
  _CONST char *src = src_void;

  if (src < dst && dst < src + length)
    {
      /* Have to copy backwards */
#ifdef TAGGED_MEMORY
      if(copyTags) {
        TAG_COPY_FORWARD(dst, src, length);
        return dst_void;
      }
#endif
      src += length;
      dst += length;
      while (length--)
	{
	  *--dst = *--src;
	}
    }
  else
    {
#ifdef TAGGED_MEMORY
      if(copyTags) {
        TAG_COPY_BACKWARD(dst, src, length);
        return dst_void;
      }
#endif
      while (length--)
	{
	  *dst++ = *src++;
	}
    }

  return dst_void;
#else
  char *dst = dst_void;
  _CONST char *src = src_void;
  long *aligned_dst;
  _CONST long *aligned_src;
  int   len =  length;

  if (src < dst && dst < src + len)
    {
#ifdef TAGGED_MEMORY
      if(copyTags) {
        TAG_COPY_BACKWARD(dst, src, length);
        return dst_void;
      }
#endif
      /* Destructive overlap...have to copy backwards */
      src += len;
      dst += len;
      while (len--)
	{
	  *--dst = *--src;
	}
    }
  else
    {
      /* Use optimizing algorithm for a non-destructive copy to closely 
         match memcpy. If the size is small or either SRC or DST is unaligned,
         then punt into the byte copy loop.  This should be rare.  */
      if (!TOO_SMALL(len) && !UNALIGNED2 (src, dst))
        {
          aligned_dst = (long*)dst;
          aligned_src = (long*)src;

          /* Copy 4X long words at a time if possible.  */
          while (len >= BIGBLOCKSIZE)
            {
#ifdef TAGGED_MEMORY
#define COPY(aligned_dst, aligned_src) do { \
  long val = *aligned_src; \
  char tag = load_tag(aligned_src); \
  *aligned_dst = val; \
  store_tag(aligned_dst, tag); \
  aligned_dst++; \
  aligned_src++; \
} while(0);

              if(copyTags) {
                COPY(aligned_dst, aligned_src);
                COPY(aligned_dst, aligned_src);
                COPY(aligned_dst, aligned_src);
                COPY(aligned_dst, aligned_src);
              } else {
#endif
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
#ifdef TAGGED_MEMORY
              }
#endif
              len -= BIGBLOCKSIZE;
            }

          /* Copy one long word at a time if possible.  */
          while (len >= LITTLEBLOCKSIZE)
            {
#ifdef TAGGED_MEMORY
              if(copyTags) {
                COPY(aligned_dst, aligned_src)
              } else {
#endif
                *aligned_dst++ = *aligned_src++;
#ifdef TAGGED_MEMORY
              }
#endif
              len -= LITTLEBLOCKSIZE;
            }

          /* Pick up any residual with a byte copier.  */
          dst = (char*)aligned_dst;
          src = (char*)aligned_src;
        }

      while (len--)
        {
          *dst++ = *src++;
        }
    }

  return dst_void;
#endif /* not PREFER_SIZE_OVER_SPEED */
}
