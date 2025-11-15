/* RISC-V 启动代码 */
    .section .text.init
    .global _start

_start:
    /* 禁用中断 */
    csrw sie, zero
    csrw sip, zero

    /* 设置栈指针 */
    /* OpenSBI已经处理了多核，我们在hart 0上运行 */
    la sp, stack_top

    /* 清空BSS段 */
    la t0, bss_start
    la t1, bss_end
1:
    bge t0, t1, 2f
    sd zero, (t0)
    addi t0, t0, 8
    j 1b
2:

    /* 跳转到C代码 */
    call kernel_main

    /* 如果返回，永久休眠 */
1:
    wfi
    j 1b

    .section .bss
    .align 16
stack_bottom:
    .skip 16384  /* 16KB栈空间 */
stack_top:
