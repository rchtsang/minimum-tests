# minimum Tests

This repository owns guest conformance programs and manifests for the `minemu`
teaching platform. It is a fork of `minimum-template` so tests exercise the same
Boot ROM, startup runtime, headers, linker layout, and build model used by the
student starter without placing platform-conformance code in that repository.

The canonical platform requirements live in the
[minemu versioned specifications](https://github.com/rchtsang/minemu/blob/main/docs/platform/abi-v1.md).
The parent
[ABI conformance matrix](https://github.com/rchtsang/minemu/blob/main/docs/dev/abi-conformance.md)
maps those requirements to Rust and guest evidence. The complete test-manifest
schema is documented in the parent
[headless-testing reference](https://github.com/rchtsang/minemu/blob/main/docs/dev/headless-testing.md).

## Prerequisites

- GNU Make and standard Unix build tools.
- `just` for the inherited student public-test workflow.
- GNU Arm Embedded tools including `arm-none-eabi-gcc`, `ar`, `objcopy`,
  `readelf`, and `nm`.
- A current `minemu` executable on `PATH`, or `MINEMU=/path/to/minemu` on Make
  or Just commands.

## Repository Structure

| Path | Ownership |
|---|---|
| `bootloader/` | Reset firmware and checked-in canonical 64-KiB Boot ROM |
| `kernel/` | Shared conformance startup/runtime, headers, linker script, baseline kernel, and examples |
| `user/` | User support library and baseline fixed-address module |
| `image/` | Baseline multi-component image and boot-handoff test |
| `headless/` | Focused single-purpose platform-conformance cases |
| `tests/` | Student-facing public black-box manifests inherited from the template |
| `docs/conformance-authoring.md` | Case layout, registration, oracles, timing, and fixtures |

Generated files stay in component-local `build/` directories.

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
make test
```

`make test` runs both conformance layers in order:

1. `image/minimum-test.toml`, which checks the canonical Boot ROM, reset path,
   initialized-data copy, BSS clearing, boot info, module metadata, and
   higher-half handoff.
2. Every focused case registered in `headless/Makefile`: `uart`, `interrupts`,
   `block`, `rng-trace`, `mmu`, `exceptions`, and `ttbr-switch`.

Override the emulator for the complete suite with:

```sh
make test MINEMU=/path/to/minemu
```

## Student Public Tests

The template's parameterized Just workflow remains available for its inherited
public manifests:

```sh
just test-all hw1
just test hw1 echo
```

These are student-homework checks, not platform-conformance checks, and are not
part of root `make test`. The baseline kernel in this repository intentionally
does not implement the HW1 shell, so its HW1 public tests are expected to fail.

## Build And Test Targets

| Command | Scope |
|---|---|
| `make` | Check Boot ROM and build kernel, user code, and examples; no image packaging or tests |
| `make image` | Package the baseline `image/build/minimum.img`; do not run it |
| `make headless` | Build all registered focused images; do not run tests |
| `make test` | Run the baseline followed by every registered focused case |
| `make -C image test` | Run the baseline only, including Boot ROM consistency check |
| `make -C headless test` | Run all focused cases, excluding the baseline |
| `make -C headless test-uart` | Run one registered focused case |
| `make -C headless image-uart` | Build one registered focused image only |
| `make -C headless/uart test` | Run one case directly from its directory |
| `just test HW NAME` | Run one inherited student public manifest |
| `just test-all HW` | Run every inherited student public manifest for one homework |

Focused-only commands use the checked-in Boot ROM but do not rebuild or compare
it. Run `make -C bootloader check` as well when validating a firmware-sensitive
change outside the complete root suite.

## Add Or Change A Case

Follow [Conformance authoring](docs/conformance-authoring.md). A focused case is
not part of aggregate build, test, or clean targets until its directory name is
added to `CASES` in `headless/Makefile`.

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

Use guest assertions for local diagnostics and an exact host manifest oracle
for pass/fail. Successful guests emit their final success trace and enter the
shared fail-stop loop; the host deadline ends execution deterministically.

## Cleaning

```sh
make clean
```

This removes generated build directories for the Boot ROM, kernel examples,
user code, baseline image, and all registered focused cases, including the
block case's disposable media. It preserves source manifests and the checked-in
`bootloader/bootloader.bin`.

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
