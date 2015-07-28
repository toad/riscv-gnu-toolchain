// See LICENSE for license details.

#ifndef LOWRISC_TAG_H
#define LOWRISC_TAG_H

#define TAG_WIDTH 4

/* Standard tag values */

/* No effect. */
#define __RISCV_TAG_NONE 0
/* Fault on write. */
#define __RISCV_TAG_READ_ONLY 1
/* Fault on read. */
#define __RISCV_TAG_WRITE_ONLY 2
/* Fault on read or write. */
#define __RISCV_TAG_INVALID 3
/* No effect, but cleared on write */
#define __RISCV_TAG_LAZY 4

int __riscv_load_tag(const void *addr);

void __riscv_store_tag(void *addr, int tag);

#endif
