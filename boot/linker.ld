/* RISC-V 链接脚本 */
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    /* 内核加载地址 */
    . = 0x80200000;

    .text : {
        *(.text.init)
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        bss_start = .;
        *(.bss*)
        *(COMMON)
        bss_end = .;
    }

    /* 内核结束标记 */
    kernel_end = .;
}
