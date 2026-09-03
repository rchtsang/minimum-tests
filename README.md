# minimum Tests

This repository is the emulator conformance fork of `minimum-template`. It
tracks the student platform layout while owning headless test programs,
manifests, and assertions that do not belong in the student starter repository.

The baseline `image/minimum-test.toml` verifies reset firmware, segment copy,
BSS clearing, boot information, and the higher-half handoff. Focused tests under
`headless/` cover UART, timers and interrupts, block media, RNG and trace, MMU
permissions and replacement bits, exception and CP15 behavior, and TTBR/TLBIALL
switching.

## Repository Structure

The five top-level source directories have distinct ownership:

```text
bootloader/  Reset firmware and the canonical 64 KiB Boot ROM binary
kernel/      Kernel headers, linker script, startup, runtime, and examples
user/        User libraries, common build support, and independent programs
image/       Packed-image manifests, generated images, and boot tests
headless/    Focused emulator conformance programs and test manifests
```

- GNU Make and standard Unix build tools.
- `just` for the parameterized public-test workflow.
- The GNU Arm Embedded toolchain, including `arm-none-eabi-gcc`,
  `arm-none-eabi-ar`, `arm-none-eabi-objcopy`, `arm-none-eabi-readelf`, and
  `arm-none-eabi-nm`.
- `minemu` version `0.2.0` on `PATH`. Verify it with `minemu --version`.

The emulator and platform documentation are maintained in the
[minemu repository](https://github.com/rchtsang/minemu). Start with the
[normative platform ABI v1](https://github.com/rchtsang/minemu/blob/main/docs/platform/abi-v1.md)
or use the [documentation index](https://github.com/rchtsang/minemu/blob/main/docs/README.md)
to find component specifications and user guides.

### Build And Package

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
- `image/minimum-test.toml` defines the corresponding headless boot test.
- `image/build/` contains generated image files.

## Building

Build the bootloader, starter kernel, user program, and all kernel examples:

```sh
make
```

Packaging is a separate step. Build the system-ROM image after the source build:

```sh
make image
```

This creates `image/build/minimum.img`. If `minemu` is not on `PATH`, provide
its executable explicitly:

```sh
make image MINEMU=/path/to/minemu
```

### Public Tests

The HW1 public tests build the current image and exercise it as a black box:

```sh
just test-all hw1
```

Use the same `MINEMU` override when the executable is not on `PATH`:

```sh
MINEMU=/path/to/minemu just test-all hw1
```

The untouched starter is expected to fail these tests because UART output,
interrupt-driven input, and the shell are student work. See
[`tests/README.md`](tests/README.md) for parameterized test commands and optional
student tests.

### Run

Boot the packaged image with the supplied 64-KiB Boot ROM:

```sh
minemu run image/build/minimum.img \
  --boot-rom bootloader/bootloader.bin
```

The TUI opens with emulation paused at the reset vector. These controls are
enough for the initial workflow:

| Input | Effect |
|---|---|
| `Space`, then `s` | Start or pause execution. |
| `:start` | Start continuous execution. |
| `:stop` | Pause execution. |
| `i` | Enter console insert mode and send keys to the selected UART. |
| `Esc` | Return to normal mode. |
| `Space`, then `i` | Open inspect view and pause before taking snapshots. |
| `Tab` | Cycle the focused inspect pane's subview. |
| `?` | Open the complete in-application help table. |
| `:q` | Shut down the emulator and quit. |

See the full [TUI guide](https://github.com/rchtsang/minemu/blob/main/docs/dev/tui.md)
for navigation, memory inspection, searches, commands, and UART selection.

## Where To Work

```text
bootloader/  Supplied reset firmware and canonical 64-KiB Boot ROM
kernel/      Starter kernel, platform headers, linker script, and examples
user/        User support library, common build rules, and starter program
image/       Image manifest and generated packaged image
tests/       Public black-box manifests and optional student tests
```

- Start kernel work in `kernel/src/core/` and use interfaces from
  `kernel/include/minemu/`.
- Preserve the supplied vector table and exception-mode stack initialization.
  Assignment 1 replaces the weak IRQ hooks by adapting the supplied
  `irq-context-switch` example.
- The released `minemu/platform.h`, `minemu/trap.h`, `minemu/irq.h`, and
  `minemu_irq_dispatch` boundary are fixed for Assignment 1. Console, buffer,
  line-reader, and shell interfaces remain student-defined.
- `minemu/block.h` provides the supplied serialized synchronous block interface;
  unit 0 is reserved for filesystem/general media and unit 1 for swap in the
  course environment.
- Add new C or assembly sources under `kernel/src/` and list their objects in
  `kernel/Makefile`; the starter intentionally does not prescribe a subsystem
  layout.
- Add kernel examples under `kernel/examples/`.
- Add independently linked user programs under `user/prog/` using the existing
  directory-local Makefile pattern.
- Select the kernel and user modules included in the image by editing
  `image/minimum.toml`.

Generated files remain in a `build/` directory beneath the component that owns
them. The repository root does not contain build output.

## Useful Targets

```sh
make kernel
make kernel-examples
make user
make image
just test-all hw1 # hw tests
make test # platform tests
make headless
make clean
```

`make test` constructs `image/build/minimum.img`, starts execution at the
platform reset vector, verifies the boot-info handoff after the kernel enables
the MMU, and checks the expected trace assertion. Override the CLI with
`MINEMU=/path/to/minemu` when needed.

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
make -C user/prog/minimum-user
```

The user program links `user/lib/build/libminimum_user.a` and `libgcc`
statically. Newlib and newlib-nano are not part of the platform.

The user-mode program and SVC example are supplied for later assignments. They
are not Assignment 1 implementation work.

## Bootloader Maintenance

Normal student work uses the checked-in `bootloader/bootloader.bin`; `make`
automatically rebuilds and compares it as a consistency check. Students should
not replace the canonical firmware. Manual firmware build, comparison, and
update instructions are documented separately in the [bootloader maintainer
guide](bootloader/README.md).

## Development Container

The supported Assignment 1 environment is published for `linux/amd64` and
`linux/arm64` as:

```text
rtsang1/cs492-stevens@sha256:9b9c8be5ccdadc046ad4577107e087aee2dad21b8fb6e081238833a70a2ef7a7
```

The corresponding readable version tag is `rtsang1/cs492-stevens:0.1.0`.
