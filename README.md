# minimum Template

This repository is the student starting point for the `minimum` teaching
operating system. It contains a starter A32 kernel, user-mode build support,
small examples, a supplied Boot ROM, and the manifest used to package a bootable
system image for `minemu`.

## Quickstart

### Prerequisites

Until the development container is released, install these tools locally:

- GNU Make and standard Unix build tools.
- The GNU Arm Embedded toolchain, including `arm-none-eabi-gcc`,
  `arm-none-eabi-ar`, `arm-none-eabi-objcopy`, `arm-none-eabi-readelf`, and
  `arm-none-eabi-nm`.
- The `minemu` executable on `PATH`.

The emulator and platform documentation are maintained in the
[minemu repository](https://github.com/rchtsang/minemu). Use its
[documentation index](https://github.com/rchtsang/minemu/blob/main/docs/README.md)
to find the current normative platform contract and user guides.

### Build And Package

From the repository root, build the kernel, user program, examples, and supplied
Boot ROM check:

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
```

- Start kernel work in `kernel/src/core/` and use interfaces from
  `kernel/include/minemu/`.
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
make clean
```

Individual examples and programs can also be built directly:

```sh
make -C kernel/examples/mmio-basics
make -C kernel/examples/svc-context-switch
make -C kernel/examples/irq-context-switch
make -C user/prog/minimum-user
```

The user program links `user/lib/build/libminimum_user.a` and `libgcc`
statically. Newlib and newlib-nano are not part of the platform.

## Bootloader Maintenance

Normal student work uses the checked-in `bootloader/bootloader.bin`; `make`
automatically rebuilds and compares it as a consistency check. Students should
not replace the canonical firmware. Manual firmware build, comparison, and
update instructions are documented separately in the [bootloader maintainer
guide](bootloader/README.md).

## Development Container

A released student development-container image is not yet available. The image
name and supported launch workflow will be documented here when the release is
published; there is currently no placeholder image to pull or run.
