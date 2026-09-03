#include "minemu/conformance.h"
#include "minemu/platform.h"

void minemu_kernel_main(const void *boot_info) {
    (void)boot_info;
    MINEMU_REQUIRE(MINEMU_RNG->state == UINT32_C(0x4d454d55), 1);
    MINEMU_REQUIRE(MINEMU_RNG->data == UINT32_C(0x791c7b62), 2);
    MINEMU_REQUIRE(MINEMU_RNG->data == UINT32_C(0x38784b1a), 3);

    MINEMU_RNG->seed = 0;
    MINEMU_REQUIRE(MINEMU_RNG->state == UINT32_C(0x4d454d55), 4);
    MINEMU_RNG->seed = UINT32_C(0x12345678);
    MINEMU_REQUIRE(MINEMU_RNG->state == UINT32_C(0x12345678), 5);
    MINEMU_REQUIRE(MINEMU_RNG->data == UINT32_C(0x87985aa5), 6);
    MINEMU_REQUIRE(MINEMU_RNG->data == UINT32_C(0x155b24a3), 7);

    minemu_trace_event(UINT32_C(0x20020001));
    minemu_trace_event(UINT32_C(0x20020002));
    minemu_trace_event(UINT32_C(0x20020003));
    minemu_fail_stop();
}
