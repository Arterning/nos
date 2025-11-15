#include <kernel/trap.h>
#include <kernel/printk.h>
#include <arch/riscv/riscv.h>

extern void trap_vector(void);

static uint64_t ticks = 0;

void trap_init(void) {
    /* 设置中断向量 */
    write_csr(stvec, (uint64_t)trap_vector);

    /* 启用时钟中断 */
    write_csr(sie, SIE_STIE);

    /* 启用全局中断 */
    write_csr(sstatus, read_csr(sstatus) | SSTATUS_SIE);

    printk("  Trap handling initialized\n");
}

void trap_handler(trapframe_t *tf) {
    uint64_t scause = tf->scause;
    uint64_t stval = tf->stval;

    if (scause & CAUSE_INTERRUPT) {
        /* 中断 */
        uint64_t int_code = scause & ~CAUSE_INTERRUPT;

        switch (int_code) {
            case CAUSE_SUPERVISOR_TIMER:
                timer_tick();
                break;
            default:
                printk("[TRAP] Unknown interrupt: %x\n", int_code);
                break;
        }
    } else {
        /* 异常 */
        printk("[TRAP] Exception!\n");
        printk("  scause: %x\n", scause);
        printk("  stval: %x\n", stval);
        printk("  sepc: %x\n", tf->sepc);

        /* 停机 */
        while (1) {
            wfi();
        }
    }
}

void timer_tick(void) {
    ticks++;

    /* 每1000个tick打印一次 */
    if (ticks % 1000 == 0) {
        printk("[TIMER] Tick: %u\n", ticks);
    }

    /* 设置下一次时钟中断 (这需要SBI支持) */
    /* 在简单实现中，我们暂时忽略 */
}
