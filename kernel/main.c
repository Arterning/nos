#include <kernel/printk.h>
#include <kernel/types.h>

/* 前向声明 */
void mm_init(void);
void trap_init(void);
void process_init(void);
void fs_init(void);
void shell_main(void);

void kernel_main(void) {
    /* 初始化内核 */
    printk("\n");
    printk("=================================\n");
    printk("  NOS - Teaching Operating System\n");
    printk("  RISC-V Edition\n");
    printk("=================================\n\n");

    printk("[KERNEL] Initializing...\n");

    /* 初始化内存管理 */
    printk("[MM] Initializing memory management...\n");
    mm_init();

    /* 初始化中断系统 */
    printk("[TRAP] Initializing interrupt handling...\n");
    trap_init();

    /* 初始化进程管理 */
    printk("[PROCESS] Initializing process scheduler...\n");
    process_init();

    /* 初始化文件系统 */
    printk("[FS] Initializing file system...\n");
    fs_init();

    printk("[KERNEL] Initialization complete!\n\n");

    /* 启动Shell */
    printk("Starting shell...\n\n");
    shell_main();

    /* 永远不应该到这里 */
    printk("[KERNEL] Shutdown.\n");
    while (1) {
        asm volatile("wfi");
    }
}
