#include <kernel/process.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/printk.h>

/* 进程表 */
static process_t proc_table[MAX_PROCESSES];
static process_t *current_proc = NULL;
static process_t *ready_queue = NULL;
static int next_pid = 1;

/* 上下文切换 (在switch.S中实现) */
extern void switch_context(context_t *old, context_t *new);

process_t *current_process(void) {
    return current_proc;
}

/* 添加进程到就绪队列 */
static void enqueue_ready(process_t *proc) {
    proc->next = NULL;
    proc->state = PROC_READY;

    if (!ready_queue) {
        ready_queue = proc;
    } else {
        process_t *p = ready_queue;
        while (p->next) {
            p = p->next;
        }
        p->next = proc;
    }
}

/* 从就绪队列获取进程 */
static process_t *dequeue_ready(void) {
    if (!ready_queue) {
        return NULL;
    }

    process_t *proc = ready_queue;
    ready_queue = proc->next;
    proc->next = NULL;
    return proc;
}

/* 创建进程 */
process_t *create_process(const char *name, void (*entry)(void)) {
    /* 查找空闲PCB */
    process_t *proc = NULL;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_UNUSED) {
            proc = &proc_table[i];
            break;
        }
    }

    if (!proc) {
        printk("[PROCESS] No free PCB\n");
        return NULL;
    }

    /* 初始化PCB */
    memset(proc, 0, sizeof(process_t));
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->priority = 1;
    strcpy(proc->name, name);

    /* 分配内核栈 */
    proc->kstack = alloc_page();
    if (!proc->kstack) {
        printk("[PROCESS] Failed to allocate kernel stack\n");
        proc->state = PROC_UNUSED;
        return NULL;
    }

    /* 设置上下文 */
    uint64_t sp = (uint64_t)proc->kstack + PAGE_SIZE;
    proc->context.ra = (uint64_t)entry;  /* 返回地址设为入口函数 */
    proc->context.sp = sp;

    /* 加入就绪队列 */
    enqueue_ready(proc);

    printk("  Created process: %s (PID %d)\n", name, proc->pid);
    return proc;
}

/* 调度器 - 时间片轮转 */
void schedule(void) {
    if (!ready_queue) {
        return;
    }

    process_t *next = dequeue_ready();
    if (!next) {
        return;
    }

    process_t *prev = current_proc;

    /* 如果当前进程仍在运行，将其放回就绪队列 */
    if (prev && prev->state == PROC_RUNNING) {
        enqueue_ready(prev);
    }

    /* 切换到新进程 */
    next->state = PROC_RUNNING;
    current_proc = next;

    if (prev && prev != next) {
        switch_context(&prev->context, &next->context);
    }
}

/* 主动让出CPU */
void yield(void) {
    if (current_proc) {
        current_proc->state = PROC_READY;
    }
    schedule();
}

/* 示例进程函数 */
static void idle_process(void) {
    while (1) {
        printk("[IDLE] Running...\n");
        for (volatile int i = 0; i < 10000000; i++);
        yield();
    }
}

static void test_process_1(void) {
    for (int i = 0; i < 5; i++) {
        printk("[PROC1] Hello from process 1! (iteration %d)\n", i);
        for (volatile int j = 0; j < 10000000; j++);
        yield();
    }
    printk("[PROC1] Process 1 finished\n");
    while (1) yield();
}

static void test_process_2(void) {
    for (int i = 0; i < 5; i++) {
        printk("[PROC2] Hello from process 2! (iteration %d)\n", i);
        for (volatile int j = 0; j < 10000000; j++);
        yield();
    }
    printk("[PROC2] Process 2 finished\n");
    while (1) yield();
}

void process_init(void) {
    /* 初始化进程表 */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        proc_table[i].state = PROC_UNUSED;
    }

    printk("  Process management initialized\n");

    /* 创建测试进程 */
    create_process("idle", idle_process);
    create_process("test1", test_process_1);
    create_process("test2", test_process_2);

    /* 启动调度 */
    schedule();
}
