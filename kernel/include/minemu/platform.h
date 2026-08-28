#ifndef MINEMU_PLATFORM_H
#define MINEMU_PLATFORM_H

#include <stddef.h>
#include <stdint.h>

#define MINEMU_BOOT_ROM_BASE UINT32_C(0x00000000)
#define MINEMU_SYSTEM_ROM_BASE UINT32_C(0x08000000)
#define MINEMU_INTERRUPT_BASE UINT32_C(0x10000000)
#define MINEMU_SYSTICK_BASE UINT32_C(0x10001000)
#define MINEMU_BLOCK_BASE UINT32_C(0x10002000)
#define MINEMU_RNG_BASE UINT32_C(0x10003000)
#define MINEMU_UART0_BASE UINT32_C(0x10004000)
#define MINEMU_UART1_BASE UINT32_C(0x10005000)
#define MINEMU_TRACE_BASE UINT32_C(0x1000f000)
#define MINEMU_RAM_BASE UINT32_C(0x40000000)
#define MINEMU_RAM_SIZE UINT32_C(0x04000000)
#define MINEMU_KERNEL_DIRECT_BASE UINT32_C(0xc0000000)

#define MINEMU_MMIO_READ32(address) (*(volatile const uint32_t *)(uintptr_t)(address))
#define MINEMU_MMIO_WRITE32(address, value) \
    (*(volatile uint32_t *)(uintptr_t)(address) = (uint32_t)(value))

#define MINEMU_UART_STATUS_RX_READY UINT32_C(1)
#define MINEMU_UART_STATUS_TX_READY UINT32_C(2)
#define MINEMU_UART_CONTROL_RX_IRQ_ENABLE UINT32_C(1)

#define MINEMU_SYSTICK_CONTROL_ENABLE UINT32_C(1)
#define MINEMU_SYSTICK_CONTROL_PERIODIC UINT32_C(2)
#define MINEMU_SYSTICK_CONTROL_IRQ_ENABLE UINT32_C(4)
#define MINEMU_SYSTICK_STATUS_PENDING UINT32_C(1)
#define MINEMU_SYSTICK_ACK UINT32_C(1)

#define MINEMU_BLOCK_COMMAND_READ UINT32_C(1)
#define MINEMU_BLOCK_COMMAND_WRITE UINT32_C(2)
#define MINEMU_BLOCK_STATUS_BUSY UINT32_C(1)
#define MINEMU_BLOCK_STATUS_COMPLETE UINT32_C(2)
#define MINEMU_BLOCK_STATUS_ERROR UINT32_C(4)
#define MINEMU_BLOCK_CONTROL_IRQ_ENABLE UINT32_C(1)
#define MINEMU_BLOCK_ACK UINT32_C(1)

#define MINEMU_IRQ_SYSTICK UINT32_C(0)
#define MINEMU_IRQ_UART0 UINT32_C(1)
#define MINEMU_IRQ_UART1 UINT32_C(2)
#define MINEMU_IRQ_BLOCK UINT32_C(3)
#define MINEMU_IRQ_NONE UINT32_C(0xffffffff)

struct __attribute__((packed, aligned(4))) minemu_interrupt_regs {
    uint32_t pending;
    uint32_t enable;
    uint32_t claim;
    uint32_t eoi;
    uint32_t priority_systick;
    uint32_t priority_uart0;
    uint32_t priority_uart1;
    uint32_t priority_block;
};

struct __attribute__((packed, aligned(4))) minemu_systick_regs {
    uint32_t period;
    uint32_t control;
    uint32_t status;
    uint32_t ack;
};

struct __attribute__((packed, aligned(4))) minemu_block_regs {
    uint32_t command;
    uint32_t lba;
    uint32_t sector_count;
    uint32_t dma_paddr;
    uint32_t status;
    uint32_t error;
    uint32_t ack;
    uint32_t control;
};

struct __attribute__((packed, aligned(4))) minemu_rng_regs {
    uint32_t seed;
    uint32_t data;
    uint32_t state;
};

struct __attribute__((packed, aligned(4))) minemu_uart_regs {
    uint32_t rx_data;
    uint32_t tx_data;
    uint32_t status;
    uint32_t control;
};

struct __attribute__((packed, aligned(4))) minemu_trace_regs {
    uint32_t event;
};

_Static_assert(sizeof(struct minemu_interrupt_regs) == 32, "interrupt register size");
_Static_assert(_Alignof(struct minemu_interrupt_regs) == 4, "interrupt register alignment");
_Static_assert(offsetof(struct minemu_interrupt_regs, claim) == 8, "interrupt CLAIM offset");
_Static_assert(offsetof(struct minemu_interrupt_regs, eoi) == 12, "interrupt EOI offset");
_Static_assert(sizeof(struct minemu_systick_regs) == 16, "systick register size");
_Static_assert(_Alignof(struct minemu_systick_regs) == 4, "systick register alignment");
_Static_assert(offsetof(struct minemu_systick_regs, ack) == 12, "systick ACK offset");
_Static_assert(sizeof(struct minemu_block_regs) == 32, "block register size");
_Static_assert(_Alignof(struct minemu_block_regs) == 4, "block register alignment");
_Static_assert(offsetof(struct minemu_block_regs, control) == 28, "block CONTROL offset");
_Static_assert(sizeof(struct minemu_rng_regs) == 12, "rng register size");
_Static_assert(_Alignof(struct minemu_rng_regs) == 4, "rng register alignment");
_Static_assert(sizeof(struct minemu_uart_regs) == 16, "uart register size");
_Static_assert(_Alignof(struct minemu_uart_regs) == 4, "uart register alignment");
_Static_assert(offsetof(struct minemu_uart_regs, control) == 12, "uart CONTROL offset");
_Static_assert(sizeof(struct minemu_trace_regs) == 4, "trace register size");
_Static_assert(_Alignof(struct minemu_trace_regs) == 4, "trace register alignment");

#define MINEMU_INTERRUPT ((volatile struct minemu_interrupt_regs *)(uintptr_t)MINEMU_INTERRUPT_BASE)
#define MINEMU_SYSTICK ((volatile struct minemu_systick_regs *)(uintptr_t)MINEMU_SYSTICK_BASE)
#define MINEMU_BLOCK ((volatile struct minemu_block_regs *)(uintptr_t)MINEMU_BLOCK_BASE)
#define MINEMU_RNG ((volatile struct minemu_rng_regs *)(uintptr_t)MINEMU_RNG_BASE)
#define MINEMU_UART0 ((volatile struct minemu_uart_regs *)(uintptr_t)MINEMU_UART0_BASE)
#define MINEMU_UART1 ((volatile struct minemu_uart_regs *)(uintptr_t)MINEMU_UART1_BASE)
#define MINEMU_TRACE ((volatile struct minemu_trace_regs *)(uintptr_t)MINEMU_TRACE_BASE)

#endif
