#include "minemu/boot.h"
#include "minemu/trap.h"
#include "minemu/trace.h"

void minemu_kernel_main(const struct minemu_boot_info *boot_info) {
    (void)boot_info;
    minemu_trace_event(1);
    minemu_fail_stop();
}
