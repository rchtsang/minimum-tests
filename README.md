# minimum Template

This repository is the starting point for the `minimum` teaching operating
system. It contains the platform bootloader, a starter kernel, user-mode build
support, reference programs, and reproducible image definitions.

## Repository Structure

The four top-level source directories have distinct ownership:

```text
bootloader/  Reset firmware and the canonical 64 KiB Boot ROM binary
kernel/      Kernel headers, linker script, startup, runtime, and examples
user/        User libraries, common build support, and independent programs
image/       Packed-image manifests, generated images, and boot tests
```

Generated artifacts remain in a `build/` directory beneath the component that
owns them. The repository root never contains build output.

### Bootloader

- `bootloader/src/` contains the reset assembly and C image loader.
- `bootloader/linker/bootloader.ld` fixes the firmware at physical address zero.
- `bootloader/bootloader.bin` is the bootloader executable that is loaded into
  minemu's bootable ROM.

### Kernel

- `kernel/include/minemu/` contains the platform and kernel interfaces available
  to kernel code.
- `kernel/src/` separates core starter code, runtime support, and
  startup assembly.
- `kernel/examples/` contains kernel-mode code examples.

### User

- `user/common/` contains the user linker script and shared Make rules.
- `user/lib/` builds user support libraries.
- `user/prog/` contains directories for independently buildable user programs.

### Image

- `image/minimum.toml` selects the kernel and user modules packed into the system
  ROM image.
- `image/build/` contains generated image files.

## Building

Build the bootloader, starter kernel, user program, and all kernel examples:

```sh
make
```

Useful component targets are:

```sh
make bootloader
make bootloader-check
make kernel
make kernel-examples
make user
make image
make clean
```

Run the packed image directly with:

```sh
minemu run image/build/minimum.img \
  --boot-rom bootloader/bootloader.bin
```

## Independent Builds

Kernel examples and user programs do not depend on root-provided path
variables. They can be compiled directly:

```sh
make -C kernel/examples/mmio-basics
make -C kernel/examples/svc-context-switch
make -C kernel/examples/irq-context-switch
make -C user/lib
make -C user/prog/minimum-user
```

The user program links `user/lib/build/libminimum_user.a` and `libgcc`
statically. New user programs should follow the same directory-local Makefile
pattern under `user/prog/`.

## Development Container

Set the released development image before entering the toolchain environment:

```sh
export MINEMU_DEVELOPMENT_IMAGE=<published-docker-image>
docker pull "$MINEMU_DEVELOPMENT_IMAGE"
docker run --rm -it -v "$PWD:/workspace" -w /workspace "$MINEMU_DEVELOPMENT_IMAGE"
```
