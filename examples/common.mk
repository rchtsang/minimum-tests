ARM_PREFIX ?= arm-none-eabi-
CC := $(ARM_PREFIX)gcc
PLATFORM_DIR ?= ../..
BUILD_DIR ?= build
CPPFLAGS := -I$(PLATFORM_DIR)/include
CFLAGS := -mcpu=cortex-a9 -marm -mfloat-abi=soft -ffreestanding -fno-builtin \
  -fdata-sections -ffunction-sections -fno-stack-protector -std=c11 -Wall -Wextra -Werror
LDFLAGS := -nostdlib -Wl,-T,$(PLATFORM_DIR)/linker/kernel.ld -Wl,--gc-sections \
  -Wl,-u,minemu_bootstrap
LIBGCC := $(shell $(CC) -mcpu=cortex-a9 -marm -mfloat-abi=soft -print-libgcc-file-name)

.PHONY: all clean

all: $(BUILD_DIR)/example.elf

$(BUILD_DIR)/example.elf: main.c $(wildcard *.S) $(PLATFORM_DIR)/build/libminemu_rt.a
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $^ $(LIBGCC) -o $@

clean:
	rm -rf $(BUILD_DIR)
