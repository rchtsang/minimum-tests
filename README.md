# minimum Template

This is the starting point for the `minimum` teaching operating system. The
root build verifies the platform Boot ROM, creates the platform runtime, a
starter kernel ELF, a starter user ELF, and complete platform reference
examples.

```sh
git clone <minimum-template-repository>
cd minimum-template
make
```

The published development image name is intentionally not fixed yet. Set it to
the released Docker Hub image before entering the development environment:

```sh
export MINEMU_DEVELOPMENT_IMAGE=<published-docker-image>
docker pull "$MINEMU_DEVELOPMENT_IMAGE"
docker run --rm -it -v "$PWD:/workspace" -w /workspace "$MINEMU_DEVELOPMENT_IMAGE"
```

Student kernel sources belong in `kernel/`; fixed-address user sources belong
in `user/`. The `examples/` directory contains complete reference
implementations and is not the student starter code.

Build and boot-test the minimal packed image with:

```sh
make system-test
```

The test starts at the platform Boot ROM reset vector, verifies the boot-info
handoff in C after the bootstrap enables the MMU, and emits a trace assertion.
Use `make system` to package without running the test, or override the CLI path
with `MINEMU=/path/to/minemu`.

Boot firmware source and its deterministic checked binary live in `bootrom/`.
Use `make bootrom` to build it or `make bootrom-check` to compare the source
build with `bootrom/minemu-bootrom.bin`. Run a packed image explicitly with:

```sh
minemu run system/build/minimum.img \
  --boot-rom bootrom/minemu-bootrom.bin
```
