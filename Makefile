ARM_PREFIX ?= arm-none-eabi-
MINEMU ?= minemu
CC := $(ARM_PREFIX)gcc
AR := $(ARM_PREFIX)ar

BUILD := build
LIB := $(BUILD)/libminemu_rt.a
CPPFLAGS := -Iinclude
CFLAGS := -mcpu=cortex-a9 -marm -mfloat-abi=soft -ffreestanding -fno-builtin \
  -fdata-sections -ffunction-sections -fno-stack-protector -std=c11 -Wall -Wextra -Werror
ASFLAGS := -mcpu=cortex-a9 -marm
RUNTIME_OBJECTS := $(BUILD)/runtime/memory.o $(BUILD)/runtime/exception.o $(BUILD)/runtime/trace.o \
  $(BUILD)/startup/boot.o $(BUILD)/startup/vectors.o
EXAMPLES := mmio-basics svc-context-switch irq-context-switch
PROJECTS := kernel user

.PHONY: all bootrom bootrom-check clean examples system system-test $(EXAMPLES) $(PROJECTS)

all: bootrom-check $(LIB) $(PROJECTS) examples

bootrom:
	$(MAKE) -C bootrom all

bootrom-check:
	$(MAKE) -C bootrom check

$(LIB): $(RUNTIME_OBJECTS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

examples: $(EXAMPLES)

kernel: $(LIB)
	$(MAKE) -C kernel PLATFORM_DIR=$(CURDIR) BUILD_DIR=$(abspath $(BUILD)/kernel)

user:
	$(MAKE) -C user PLATFORM_DIR=$(CURDIR) BUILD_DIR=$(abspath $(BUILD)/user)

$(EXAMPLES): $(LIB)
	$(MAKE) -C examples/$@ PLATFORM_DIR=$(CURDIR) BUILD_DIR=$(abspath $(BUILD)/examples/$@)

system: bootrom-check kernel user
	$(MAKE) -C system MINEMU="$(MINEMU)" all

system-test: system
	$(MAKE) -C system MINEMU="$(MINEMU)" test

clean:
	$(MAKE) -C bootrom clean
	rm -rf $(BUILD)
