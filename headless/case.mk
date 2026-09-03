MINEMU ?= minemu

include ../../kernel/common.mk

IMAGE := build/$(PROGRAM).img

.PHONY: image test prepare

image: $(IMAGE)

prepare:

$(IMAGE): image.toml $(ELF)
	$(MINEMU) image image.toml --output $@

test: prepare $(IMAGE) test.toml
	$(MINEMU) test test.toml
