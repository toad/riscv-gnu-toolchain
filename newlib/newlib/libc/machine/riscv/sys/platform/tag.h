// See LICENSE for license details.

#ifndef LOWRISC_TAG_H
#define LOWRISC_TAG_H

#define TAG_WIDTH 4

int __riscv_load_tag(const void *addr);

void __riscv_store_tag(void *addr, int tag);

#endif
