#include <kernel/shell.h>
#include <kernel/printk.h>
#include <kernel/string.h>
#include <kernel/fs.h>
#include <kernel/process.h>
#include <kernel/mm.h>

#define CMD_BUF_SIZE 256
#define MAX_ARGS 16

/* UART输入 */
#define UART_BASE 0x10000000UL
#define UART_RHR (*(volatile uint8_t *)(UART_BASE + 0))
#define UART_LSR (*(volatile uint8_t *)(UART_BASE + 5))

static char getchar_blocking(void) {
    /* 等待数据可用 */
    while ((UART_LSR & 0x01) == 0);
    return UART_RHR;
}

/* 读取一行输入 */
static void readline(char *buf, int maxlen) {
    int i = 0;

    while (i < maxlen - 1) {
        char c = getchar_blocking();

        if (c == '\r' || c == '\n') {
            putchar('\n');
            break;
        } else if (c == 127 || c == '\b') {  /* Backspace */
            if (i > 0) {
                i--;
                puts("\b \b");  /* 退格并清除 */
            }
        } else if (c >= 32 && c < 127) {  /* 可打印字符 */
            buf[i++] = c;
            putchar(c);
        }
    }

    buf[i] = '\0';
}

/* 分割命令行参数 */
static int parse_args(char *cmd, char **argv) {
    int argc = 0;
    char *p = cmd;

    while (*p && argc < MAX_ARGS) {
        /* 跳过空格 */
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '\0') {
            break;
        }

        /* 记录参数 */
        argv[argc++] = p;

        /* 查找下一个空格 */
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }

        if (*p) {
            *p++ = '\0';
        }
    }

    return argc;
}

/* 命令: help */
static void cmd_help(void) {
    printk("Available commands:\n");
    printk("  help       - Show this help message\n");
    printk("  ls         - List files\n");
    printk("  cat <file> - Display file contents\n");
    printk("  ps         - List processes\n");
    printk("  mem        - Show memory info\n");
    printk("  echo <msg> - Print a message\n");
    printk("  clear      - Clear screen\n");
    printk("  about      - About NOS\n");
}

/* 命令: ls */
static void cmd_ls(void) {
    fs_list();
}

/* 命令: cat */
static void cmd_cat(int argc, char **argv) {
    if (argc < 2) {
        printk("Usage: cat <filename>\n");
        return;
    }

    char buf[1024];
    int size = fs_read(argv[1], buf, sizeof(buf) - 1);

    if (size < 0) {
        printk("Failed to read file: %s\n", argv[1]);
        return;
    }

    buf[size] = '\0';
    printk("%s\n", buf);
}

/* 命令: ps */
static void cmd_ps(void) {
    printk("Process list:\n");
    printk("  PID  %-16s  State\n", "Name");
    printk("  ----------------------------------\n");

    /* 简化版：只显示当前进程 */
    process_t *proc = current_process();
    if (proc) {
        const char *state_str[] = {
            "UNUSED", "READY", "RUNNING", "SLEEPING", "ZOMBIE"
        };
        printk("  %-4d %-16s  %s\n",
               proc->pid, proc->name, state_str[proc->state]);
    }

    printk("\n  Note: Full process list not implemented yet\n");
}

/* 命令: mem */
static void cmd_mem(void) {
    extern uint64_t get_free_pages(void);
    uint64_t free = get_free_pages();

    printk("Memory information:\n");
    printk("  Free pages: %u\n", free);
    printk("  Free memory: %u KB\n", free * 4);
}

/* 命令: echo */
static void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        printk("%s", argv[i]);
        if (i < argc - 1) {
            printk(" ");
        }
    }
    printk("\n");
}

/* 命令: clear */
static void cmd_clear(void) {
    printk("\033[2J\033[H");  /* ANSI清屏 */
}

/* 命令: about */
static void cmd_about(void) {
    printk("\n");
    printk("=================================\n");
    printk("  NOS - Teaching Operating System\n");
    printk("  RISC-V Edition\n");
    printk("=================================\n\n");
    printk("A simple operating system for educational purposes.\n\n");
    printk("Features:\n");
    printk("  - RISC-V architecture support\n");
    printk("  - Physical/Virtual memory management\n");
    printk("  - Process scheduling (round-robin)\n");
    printk("  - Simple in-memory file system\n");
    printk("  - Basic shell with commands\n\n");
}

/* 执行命令 */
static void execute_command(char *cmd) {
    char *argv[MAX_ARGS];
    int argc = parse_args(cmd, argv);

    if (argc == 0) {
        return;
    }

    /* 命令分发 */
    if (strcmp(argv[0], "help") == 0) {
        cmd_help();
    } else if (strcmp(argv[0], "ls") == 0) {
        cmd_ls();
    } else if (strcmp(argv[0], "cat") == 0) {
        cmd_cat(argc, argv);
    } else if (strcmp(argv[0], "ps") == 0) {
        cmd_ps();
    } else if (strcmp(argv[0], "mem") == 0) {
        cmd_mem();
    } else if (strcmp(argv[0], "echo") == 0) {
        cmd_echo(argc, argv);
    } else if (strcmp(argv[0], "clear") == 0) {
        cmd_clear();
    } else if (strcmp(argv[0], "about") == 0) {
        cmd_about();
    } else {
        printk("Unknown command: %s\n", argv[0]);
        printk("Type 'help' for available commands.\n");
    }
}

/* Shell主循环 */
void shell_main(void) {
    char cmd_buf[CMD_BUF_SIZE];

    printk("NOS Shell v1.0\n");
    printk("Type 'help' for available commands.\n\n");

    while (1) {
        printk("nos> ");
        readline(cmd_buf, CMD_BUF_SIZE);
        execute_command(cmd_buf);
    }
}
