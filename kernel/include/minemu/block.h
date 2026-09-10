#ifndef MINEMU_BLOCK_H
#define MINEMU_BLOCK_H

#include <stdint.h>

uint32_t minemu_block_read(uint32_t unit, uint32_t lba, uint32_t sector_count,
                           uint32_t dma_paddr);
uint32_t minemu_block_write(uint32_t unit, uint32_t lba, uint32_t sector_count,
                            uint32_t dma_paddr);

#endif
