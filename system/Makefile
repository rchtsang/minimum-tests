MINEMU ?= minemu
IMAGE := build/minimum.img

.PHONY: all test clean

all:
	mkdir -p build
	$(MINEMU) image minimum.toml --output $(IMAGE)

test: all
	$(MINEMU) test minimum-test.toml

clean:
	rm -rf build
