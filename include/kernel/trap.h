#ifndef _KERNEL_TRAP_H
#define _KERNEL_TRAP_H

#include <kernel/types.h>

/* 陷阱帧 - 保存寄存器上下文 */
typedef struct {
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0, t1, t2;
    uint64_t s0, s1;
    uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
    uint64_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    uint64_t t3, t4, t5, t6;
    uint64_t sepc;    /* 异常发生时的PC */
    uint64_t sstatus; /* 状态寄存器 */
    uint64_t scause;  /* 异常原因 */
    uint64_t stval;   /* 附加信息 */
} trapframe_t;

/* 中断/异常原因 */
#define CAUSE_INTERRUPT (1ULL << 63)
#define CAUSE_SUPERVISOR_TIMER 5

/* 初始化中断系统 */
void trap_init(void);

/* 时钟中断处理 */
void timer_tick(void);

#endif
