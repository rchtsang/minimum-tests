ARM_PREFIX ?= arm-none-eabi-
CC := $(ARM_PREFIX)gcc
PLATFORM_DIR ?= ..
BUILD_DIR ?= build
CPPFLAGS := -I$(PLATFORM_DIR)/include
CFLAGS := -mcpu=cortex-a9 -marm -mfloat-abi=soft -ffreestanding -fno-builtin \
  -fdata-sections -ffunction-sections -fno-stack-protector -std=c11 -Wall -Wextra -Werror
LDFLAGS := -nostdlib -Wl,-T,$(PLATFORM_DIR)/linker/user.ld -Wl,--gc-sections
LIBGCC := $(shell $(CC) -mcpu=cortex-a9 -marm -mfloat-abi=soft -print-libgcc-file-name)

.PHONY: all clean

all: $(BUILD_DIR)/minimum-user.elf

$(BUILD_DIR)/minimum-user.elf: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< $(LIBGCC) -o $@

clean:
	rm -rf $(BUILD_DIR)
