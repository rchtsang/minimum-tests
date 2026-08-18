# minemu Boot ROM

This fixed A32 firmware is the platform reset implementation supplied by the
minimum teaching platform. The assembly
entry establishes supervisor state and a temporary stack; the C loader copies
validated kernel segments from system ROM, clears BSS, publishes boot info, and
hands control to the physical kernel bootstrap.

`minemu-bootrom.bin` is checked in so using the template does not rebuild
firmware implicitly and deterministic guest timing does not depend on the local
compiler version. Regenerate and compare it explicitly with:

```sh
make -C bootrom update
make -C bootrom check
```
