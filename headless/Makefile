MINEMU ?= minemu

CASES := uart interrupts block block-unattached rng-trace mmu exceptions ttbr-switch
IMAGE_TARGETS := $(addprefix image-,$(CASES))
TEST_TARGETS := $(addprefix test-,$(CASES))

.PHONY: images test clean kernel-runtime $(IMAGE_TARGETS) $(TEST_TARGETS)

images: $(IMAGE_TARGETS)

test: $(TEST_TARGETS)

kernel-runtime:
	$(MAKE) -C ../kernel runtime

$(IMAGE_TARGETS) $(TEST_TARGETS): | kernel-runtime

$(IMAGE_TARGETS): image-%:
	$(MAKE) -C $* MINEMU="$(MINEMU)" KERNEL_RUNTIME_READY=1 image

$(TEST_TARGETS): test-%:
	$(MAKE) -C $* MINEMU="$(MINEMU)" KERNEL_RUNTIME_READY=1 test

clean:
	@for case in $(CASES); do $(MAKE) -C $$case clean; done
