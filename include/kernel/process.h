#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <kernel/types.h>

/* 进程状态 */
typedef enum {
    PROC_UNUSED = 0,  /* 未使用 */
    PROC_READY,       /* 就绪 */
    PROC_RUNNING,     /* 运行中 */
    PROC_SLEEPING,    /* 睡眠 */
    PROC_ZOMBIE       /* 僵尸 */
} proc_state_t;

/* 进程上下文 */
typedef struct {
    uint64_t ra;
    uint64_t sp;
    uint64_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
} context_t;

/* 进程控制块 */
#define MAX_PROCESSES 16
#define PROC_NAME_LEN 32

typedef struct process {
    int pid;                    /* 进程ID */
    proc_state_t state;         /* 状态 */
    char name[PROC_NAME_LEN];   /* 进程名 */

    context_t context;          /* 上下文 */
    void *kstack;               /* 内核栈 */

    uint64_t runtime;           /* 运行时间 */
    int priority;               /* 优先级 */

    struct process *next;       /* 下一个进程 */
} process_t;

/* 进程管理函数 */
void process_init(void);
process_t *create_process(const char *name, void (*entry)(void));
void schedule(void);
process_t *current_process(void);
void yield(void);

#endif
