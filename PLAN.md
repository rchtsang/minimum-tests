# Repository Reorganization Plan

## Goals

Reorganize the repository around clear ownership boundaries, simplify the
Makefiles, keep generated artifacts with their owning components, and make the
layout understandable to students.

The repository will have exactly four non-hidden top-level directories:

```text
bootloader/
kernel/
user/
image/
```

Root-level files such as `README.md`, `Makefile`, and `.gitignore` remain at the
repository root.

## Target Structure

```text
bootloader/
  Makefile
  README.md
  bootloader.bin
  linker/
    bootloader.ld
  src/
    boot.c
    start.S
  build/

kernel/
  Makefile
  common.mk
  include/
    minemu/
  linker/
    kernel.ld
  src/
    core/
      main.c
    runtime/
      exception.c
      memory.c
      trace.c
    startup/
      boot.S
      vectors.S
  examples/
    mmio-basics/
    svc-context-switch/
    irq-context-switch/
  build/

user/
  Makefile
  common/
    common.mk
    user.ld
  lib/
    Makefile
    include/
    src/
      memory.c
    build/
  prog/
    minimum-user/
      Makefile
      main.c
      build/

image/
  Makefile
  minimum.toml
  minimum-test.toml
  build/
    minimum.img
```

## File Migration

Move the existing files as follows:

| Current path | New path |
|---|---|
| `bootrom/Makefile` | `bootloader/Makefile` |
| `bootrom/README.md` | `bootloader/README.md` |
| `bootrom/boot.c` | `bootloader/src/boot.c` |
| `bootrom/start.S` | `bootloader/src/start.S` |
| `bootrom/bootrom.ld` | `bootloader/linker/bootloader.ld` |
| `bootrom/minemu-bootrom.bin` | `bootloader/bootloader.bin` |
| `include/minemu/*` | `kernel/include/minemu/*` |
| `linker/kernel.ld` | `kernel/linker/kernel.ld` |
| `kernel/main.c` | `kernel/src/core/main.c` |
| `runtime/*` | `kernel/src/runtime/*` |
| `startup/*` | `kernel/src/startup/*` |
| `examples/common.mk` | `kernel/common.mk` |
| `examples/*` | `kernel/examples/*` |
| `linker/user.ld` | `user/common/user.ld` |
| `user/main.c` | `user/prog/minimum-user/main.c` |
| `system/Makefile` | `image/Makefile` |
| `system/minimum.toml` | `image/minimum.toml` |
| `system/minimum-test.toml` | `image/minimum-test.toml` |

The generated and currently tracked `build/libminemu_rt.a` will be removed
from version control. The only generated binary retained in version control
will be the reproducible canonical ROM image at
`bootloader/bootloader.bin`.

## Build Organization

### Root Makefile

The root Makefile will only orchestrate component builds. It will not compile
sources or create a root `build/` directory.

The supported targets will be:

```text
make
make bootloader
make bootloader-check
make kernel
make kernel-examples
make user
make image
make test
make clean
```

The old `bootrom`, `system`, and `system-test` targets will be removed rather
than retained as compatibility aliases.

### Bootloader

`bootloader/Makefile` will own all bootloader compilation, linking, binary
generation, and reproducibility checks. Its build products will remain under
`bootloader/build/`.

The `check` target will verify that:

- The ELF entry point is zero.
- The ELF has no undefined symbols.
- The generated ROM is exactly 64 KiB.
- The generated code is A32 rather than Thumb.
- The generated ROM exactly matches `bootloader/bootloader.bin`.

### Kernel

`kernel/Makefile` will own:

- Kernel startup objects.
- Kernel runtime objects.
- The kernel runtime static archive.
- The starter kernel ELF.
- Aggregation of kernel-mode examples.

The starter kernel output will be named
`kernel/build/minimum-kernel.elf`. Kernel runtime output will also remain under
`kernel/build/`.

Kernel examples will live under `kernel/examples/<name>/`, use descriptive ELF
names, and support direct independent builds such as:

```sh
make -C kernel/examples/mmio-basics
```

Shared kernel and kernel-example rules will derive repository paths from their
own Makefile locations rather than requiring caller-provided `PLATFORM_DIR` or
`BUILD_DIR` variables.

### User Programs and Library

`user/lib` will initially provide statically linked implementations of:

- `memcpy`
- `memset`
- `memmove`
- `memcmp`

The library will build as `user/lib/build/libminimum_user.a`.

Every program under `user/prog/<name>/` will have an independently usable
Makefile, produce its output in a local `build/` directory, and link the user
library and `libgcc` statically. For example:

```sh
make -C user/prog/minimum-user
```

`user/Makefile` will aggregate the library and all user programs.

### Images

`image/Makefile` will own packed image generation and boot testing.
`image/build/minimum.img` will be a real file target with explicit dependencies
on the manifest, starter kernel ELF, and starter user ELF. Testing will depend
on that target without packaging the image a second time.

The image manifest will reference:

```toml
kernel = "../kernel/build/minimum-kernel.elf"

[[modules]]
name = "minimum-user"
elf = "../user/prog/minimum-user/build/minimum-user.elf"
```

The test manifest will reference:

```toml
image = "build/minimum.img"
boot_rom = "../bootloader/bootloader.bin"
```

## Dependency Tracking

All C and assembly builds will generate dependency files with `-MMD -MP` where
supported. Headers and linker scripts will be explicit prerequisites so their
changes trigger the appropriate rebuilds and relinks.

Recursive component rules will ensure that direct builds work without first
running the root Makefile. For example, a user program may request the user
library build, and an image build may request its kernel and user inputs.

## Generated Artifacts

No build directories or build artifacts will exist at the repository root.
Generated files will remain under the component that owns them:

- `bootloader/build/`
- `kernel/build/`
- `kernel/examples/*/build/`
- `user/lib/build/`
- `user/prog/*/build/`
- `image/build/`

The repository `.gitignore` will cover component-local object files,
dependency files, archives, ELF files, map files, packed images, and build
directories. `bootloader/bootloader.bin` will remain tracked as the canonical
reproducible ROM.

## Bootloader Documentation

The bootloader source will be documented for students. Comments will explain:

- The reset vector table and unexpected-vector behavior.
- Entry into A32 supervisor mode.
- Temporary stack selection before RAM-resident kernel startup.
- Packed image header and kernel segment records.
- Explicit little-endian volatile reads and writes.
- Kernel segment copying and BSS clearing.
- Boot-info construction and physical placement.
- The higher-half boot-info pointer passed to the kernel bootstrap.
- The final transfer from ROM firmware to the loaded kernel.
- Assumptions that are guaranteed by the image packer and platform ABI.

Comments will focus on hardware, image-format, and ABI reasoning rather than
merely restating individual instructions or assignments.

## README

The root README will document:

- The four-directory repository structure.
- Ownership boundaries between bootloader, kernel, user code, and images.
- Root build and test commands.
- Independent kernel-example and user-program builds.
- Static user-library linkage.
- Generated output locations.
- Bootloader reproducibility checks.
- Image construction and headless boot testing.
- Development-container usage.

## Parent Repository Updates

References in the parent `min-emu` repository will be updated for the new
paths and targets, including:

- The root `justfile`.
- Parent ignore rules.
- CLI help examples.
- Developer documentation.
- TUI launch examples.
- Bootloader ownership documentation.
- References to the platform headers.

Old references to `bootrom/`, `system/`, the root template `build/` directory,
and `minemu-bootrom.bin` will be removed.

## Verification

The reorganization will be verified with direct component builds:

```sh
make -C bootloader check
make -C kernel
make -C kernel/examples/mmio-basics
make -C kernel/examples/svc-context-switch
make -C kernel/examples/irq-context-switch
make -C user/lib
make -C user/prog/minimum-user
make image
make test
```

The parent repository CI will then be run:

```sh
just ci
```

Final checks will confirm that:

- No root `build/` directory is created.
- Every component can be built directly.
- Header and linker-script changes have correct dependencies.
- The canonical bootloader binary is reproducible.
- The packed image boots and passes its assertions.
- Generated outputs are ignored by Git.
