# Conformance Bootloader Fixture

This directory mirrors the Boot ROM shipped by `minimum-template` so conformance
images exercise the same reset firmware as student images. Do not evolve this
copy independently. Apply intentional firmware changes in `minimum-template`,
then synchronize the source and canonical `bootloader.bin` here.

Conformance maintainers can rebuild and compare the synchronized fixture with:

```sh
make -C bootloader
make -C bootloader check
```

See the [conformance repository overview](../README.md) for full-suite workflows.
