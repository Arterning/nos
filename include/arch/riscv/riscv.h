#ifndef _ARCH_RISCV_H
#define _ARCH_RISCV_H

#include <kernel/types.h>

/* RISC-V 寄存器定义 */
#define SATP_MODE_SV39 (8ULL << 60)
#define PAGE_SIZE 4096

/* CSR寄存器操作 */
#define read_csr(reg) ({ \
    uint64_t __val; \
    asm volatile("csrr %0, " #reg : "=r"(__val)); \
    __val; \
})

#define write_csr(reg, val) ({ \
    asm volatile("csrw " #reg ", %0" :: "r"(val)); \
})

/* 中断相关 */
#define SSTATUS_SIE (1UL << 1)  /* Supervisor Interrupt Enable */
#define SSTATUS_SPIE (1UL << 5) /* Previous SIE */
#define SIE_STIE (1UL << 5)     /* Timer Interrupt Enable */

/* 内存屏障 */
static inline void sfence_vma(void) {
    asm volatile("sfence.vma" ::: "memory");
}

/* 等待中断 */
static inline void wfi(void) {
    asm volatile("wfi");
}

#endif
