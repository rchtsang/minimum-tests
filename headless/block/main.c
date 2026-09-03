#include <stddef.h>

#include "minemu/conformance.h"
#include "minemu/platform.h"

#define BLOCK_ERROR_NONE UINT32_C(0)
#define BLOCK_ERROR_BUSY UINT32_C(2)
#define BLOCK_ERROR_INVALID_COMMAND UINT32_C(3)
#define BLOCK_ERROR_INVALID_DMA UINT32_C(4)
#define BLOCK_ERROR_INVALID_LBA UINT32_C(5)

static uint8_t dma[512] __attribute__((aligned(512)));

extern uint64_t minemu_block_exact_32(uint32_t operation);

static void wait_complete(uint32_t expected_error) {
    while ((MINEMU_BLOCK->status & MINEMU_BLOCK_STATUS_COMPLETE) == 0U) {
    }
    if (MINEMU_BLOCK->error != expected_error) {
        minemu_trace_event(
            UINT32_C(0xe0030000) | (expected_error << 8) | MINEMU_BLOCK->error);
        minemu_conformance_fail(10 + expected_error);
    }
    MINEMU_REQUIRE(
        ((MINEMU_BLOCK->status & MINEMU_BLOCK_STATUS_ERROR) != 0U) ==
            (expected_error != BLOCK_ERROR_NONE),
        20 + expected_error);
    MINEMU_BLOCK->ack = MINEMU_BLOCK_ACK;
}

static void command(uint32_t operation, uint32_t lba, uint32_t sectors, uint32_t address) {
    MINEMU_BLOCK->lba = lba;
    MINEMU_BLOCK->sector_count = sectors;
    MINEMU_BLOCK->dma_paddr = address;
    MINEMU_BLOCK->command = operation;
}

static void exact_command(uint32_t operation, uint32_t lba, uint32_t sectors, uint32_t address,
                          uint32_t expected_error) {
    MINEMU_BLOCK->lba = lba;
    MINEMU_BLOCK->sector_count = sectors;
    MINEMU_BLOCK->dma_paddr = address;
    uint64_t samples = minemu_block_exact_32(operation);
    MINEMU_REQUIRE((uint32_t)samples == MINEMU_BLOCK_STATUS_BUSY, 6);
    uint32_t completion = (uint32_t)(samples >> 32);
    MINEMU_REQUIRE((completion & MINEMU_BLOCK_STATUS_BUSY) == 0U &&
                       (completion & MINEMU_BLOCK_STATUS_COMPLETE) != 0U,
                   7);
    wait_complete(expected_error);
}

void minemu_kernel_main(const void *boot_info) {
    (void)boot_info;
    uint32_t dma_paddr = minemu_test_paddr(dma);

    exact_command(MINEMU_BLOCK_COMMAND_READ, 0, 1, dma_paddr, BLOCK_ERROR_NONE);
    for (size_t i = 0; i < sizeof(dma); ++i) {
        MINEMU_REQUIRE(dma[i] == 0, 2);
        dma[i] = UINT8_C(0xa5);
    }

    exact_command(MINEMU_BLOCK_COMMAND_WRITE, 1, 1, dma_paddr, BLOCK_ERROR_NONE);

    for (size_t i = 0; i < sizeof(dma); ++i) {
        dma[i] = 0;
    }
    exact_command(MINEMU_BLOCK_COMMAND_READ, 1, 1, dma_paddr, BLOCK_ERROR_NONE);
    for (size_t i = 0; i < sizeof(dma); ++i) {
        MINEMU_REQUIRE(dma[i] == UINT8_C(0xa5), 4);
    }

    exact_command(UINT32_C(99), 0, 1, dma_paddr, BLOCK_ERROR_INVALID_COMMAND);
    exact_command(MINEMU_BLOCK_COMMAND_READ, 0, 1, dma_paddr + 1U, BLOCK_ERROR_INVALID_DMA);
    exact_command(MINEMU_BLOCK_COMMAND_READ, 2, 1, dma_paddr, BLOCK_ERROR_INVALID_LBA);

    command(MINEMU_BLOCK_COMMAND_READ, 0, 1, dma_paddr);
    MINEMU_BLOCK->command = MINEMU_BLOCK_COMMAND_READ;
    MINEMU_REQUIRE(MINEMU_BLOCK->error == BLOCK_ERROR_BUSY, 5);
    MINEMU_BLOCK->ack = MINEMU_BLOCK_ACK;
    wait_complete(BLOCK_ERROR_NONE);

    minemu_trace_event(UINT32_C(0x20030001));
    minemu_fail_stop();
}
