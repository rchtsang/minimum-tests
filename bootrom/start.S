.syntax unified
.cpu cortex-a9
.arm

.section .vectors, "ax", %progbits
.global _start
.type _start, %function
_start:
    b reset
    b fail_stop
    b fail_stop
    b fail_stop
    b fail_stop
    b fail_stop
    b fail_stop
    b fail_stop

.section .text.startup, "ax", %progbits
.type reset, %function
reset:
    msr cpsr_c, #0xd3
    ldr sp, =0x40007000
    b boot_main

.global fail_stop
.type fail_stop, %function
fail_stop:
    b fail_stop
