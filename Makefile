MINEMU ?= minemu
A1_TESTS := tests/assignment1/hello-world.toml \
	tests/assignment1/prompt.toml tests/assignment1/echo.toml

.PHONY: all bootloader bootloader-check kernel kernel-examples user image test \
	test-a1-hello-world test-a1-prompt test-a1-echo clean

all: bootloader-check kernel user kernel-examples

bootloader:
	$(MAKE) -C bootloader all

bootloader-check:
	$(MAKE) -C bootloader check

kernel:
	$(MAKE) -C kernel all

kernel-examples:
	$(MAKE) -C kernel examples

user:
	$(MAKE) -C user all

image: kernel user
	$(MAKE) -C image MINEMU="$(MINEMU)" all

test: image
	@status=0; for manifest in $(A1_TESTS); do \
		printf '==> %s\n' "$$manifest"; \
		$(MINEMU) test "$$manifest" || status=$$?; \
	done; exit $$status

test-a1-hello-world: image
	$(MINEMU) test tests/assignment1/hello-world.toml

test-a1-prompt: image
	$(MINEMU) test tests/assignment1/prompt.toml

test-a1-echo: image
	$(MINEMU) test tests/assignment1/echo.toml

clean:
	$(MAKE) -C bootloader clean
	$(MAKE) -C kernel clean
	$(MAKE) -C user clean
	$(MAKE) -C image clean
