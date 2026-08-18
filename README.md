# minimum Template

This is the starting point for the `minimum` teaching operating system. The
root build creates the platform runtime, a starter kernel ELF, a starter user
ELF, and complete platform reference examples.

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
