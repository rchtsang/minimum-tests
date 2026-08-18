#ifndef MINEMU_BOOT_H
#define MINEMU_BOOT_H

#include <stdint.h>

#define MINEMU_BOOT_INFO_MAGIC UINT32_C(0x4d424f4f)
#define MINEMU_ABI_VERSION UINT16_C(1)
#define MINEMU_BOOT_INFO_VADDR UINT32_C(0xc0007000)

struct minemu_boot_info {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t system_rom_base;
    uint32_t image_size;
    uint32_t module_table_offset;
    uint32_t module_count;
    uint32_t direct_map_vaddr;
    uint32_t direct_map_paddr;
    uint32_t direct_map_size;
    uint32_t flags;
    uint32_t reserved[6];
};

_Static_assert(sizeof(struct minemu_boot_info) == 64, "boot-info size");

#endif
