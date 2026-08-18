#include <stdint.h>

#define SYSTEM_ROM_BASE UINT32_C(0x08000000)
#define BOOT_INFO_PADDR UINT32_C(0x40007000)
#define BOOT_INFO_VADDR UINT32_C(0xc0007000)

#define IMAGE_SIZE_OFFSET UINT32_C(0x08)
#define KERNEL_TABLE_OFFSET UINT32_C(0x0c)
#define KERNEL_COUNT_OFFSET UINT32_C(0x10)
#define MODULE_TABLE_OFFSET UINT32_C(0x14)
#define MODULE_COUNT_OFFSET UINT32_C(0x18)
#define BOOTSTRAP_ENTRY_OFFSET UINT32_C(0x1c)

#define SEGMENT_DATA_OFFSET UINT32_C(0x00)
#define SEGMENT_PADDR_OFFSET UINT32_C(0x04)
#define SEGMENT_FILE_SIZE_OFFSET UINT32_C(0x0c)
#define SEGMENT_MEMORY_SIZE_OFFSET UINT32_C(0x10)
#define KERNEL_SEGMENT_SIZE UINT32_C(32)

#define BOOT_INFO_MAGIC UINT32_C(0x4d424f4f)
#define ABI_VERSION UINT32_C(1)
#define BOOT_INFO_SIZE UINT32_C(64)
#define DIRECT_MAP_VADDR UINT32_C(0xc0000000)
#define DIRECT_MAP_PADDR UINT32_C(0x40000000)
#define DIRECT_MAP_SIZE UINT32_C(0x04000000)

typedef void (*bootstrap_entry)(const void *boot_info);

static uint32_t read_u32(uint32_t address) {
    const volatile uint8_t *bytes = (const volatile uint8_t *)(uintptr_t)address;
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static void write_u16(uint32_t address, uint16_t value) {
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)address;
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

static void write_u32(uint32_t address, uint32_t value) {
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)address;
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

static void copy_bytes(uint32_t destination, uint32_t source, uint32_t count) {
    volatile uint8_t *to = (volatile uint8_t *)(uintptr_t)destination;
    const volatile uint8_t *from = (const volatile uint8_t *)(uintptr_t)source;
    for (uint32_t index = 0; index < count; ++index) {
        to[index] = from[index];
    }
}

static void zero_bytes(uint32_t destination, uint32_t count) {
    volatile uint8_t *bytes = (volatile uint8_t *)(uintptr_t)destination;
    for (uint32_t index = 0; index < count; ++index) {
        bytes[index] = 0;
    }
}

static void write_boot_info(uint32_t image_size, uint32_t module_table,
                            uint32_t module_count) {
    zero_bytes(BOOT_INFO_PADDR, BOOT_INFO_SIZE);
    write_u32(BOOT_INFO_PADDR + 0x00, BOOT_INFO_MAGIC);
    write_u16(BOOT_INFO_PADDR + 0x04, (uint16_t)ABI_VERSION);
    write_u16(BOOT_INFO_PADDR + 0x06, (uint16_t)BOOT_INFO_SIZE);
    write_u32(BOOT_INFO_PADDR + 0x08, SYSTEM_ROM_BASE);
    write_u32(BOOT_INFO_PADDR + 0x0c, image_size);
    write_u32(BOOT_INFO_PADDR + 0x10, module_table);
    write_u32(BOOT_INFO_PADDR + 0x14, module_count);
    write_u32(BOOT_INFO_PADDR + 0x18, DIRECT_MAP_VADDR);
    write_u32(BOOT_INFO_PADDR + 0x1c, DIRECT_MAP_PADDR);
    write_u32(BOOT_INFO_PADDR + 0x20, DIRECT_MAP_SIZE);
}

__attribute__((noreturn)) void boot_main(void) {
    const uint32_t image_size = read_u32(SYSTEM_ROM_BASE + IMAGE_SIZE_OFFSET);
    const uint32_t kernel_table = read_u32(SYSTEM_ROM_BASE + KERNEL_TABLE_OFFSET);
    const uint32_t kernel_count = read_u32(SYSTEM_ROM_BASE + KERNEL_COUNT_OFFSET);

    for (uint32_t index = 0; index < kernel_count; ++index) {
        const uint32_t record = SYSTEM_ROM_BASE + kernel_table + index * KERNEL_SEGMENT_SIZE;
        const uint32_t source =
            SYSTEM_ROM_BASE + read_u32(record + SEGMENT_DATA_OFFSET);
        const uint32_t destination = read_u32(record + SEGMENT_PADDR_OFFSET);
        const uint32_t file_size = read_u32(record + SEGMENT_FILE_SIZE_OFFSET);
        const uint32_t memory_size = read_u32(record + SEGMENT_MEMORY_SIZE_OFFSET);
        copy_bytes(destination, source, file_size);
        zero_bytes(destination + file_size, memory_size - file_size);
    }

    write_boot_info(image_size,
                    read_u32(SYSTEM_ROM_BASE + MODULE_TABLE_OFFSET),
                    read_u32(SYSTEM_ROM_BASE + MODULE_COUNT_OFFSET));

    const bootstrap_entry entry = (bootstrap_entry)(uintptr_t)read_u32(
        SYSTEM_ROM_BASE + BOOTSTRAP_ENTRY_OFFSET);
    entry((const void *)(uintptr_t)BOOT_INFO_VADDR);
    __builtin_unreachable();
}
