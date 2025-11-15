#include <kernel/printk.h>
#include <kernel/types.h>

/* UART 基地址 (QEMU RISC-V virt) */
#define UART_BASE 0x10000000UL

/* UART 寄存器 */
#define UART_THR (*(volatile uint8_t *)(UART_BASE + 0)) /* 发送保持寄存器 */
#define UART_LSR (*(volatile uint8_t *)(UART_BASE + 5)) /* 线路状态寄存器 */

void putchar(char c) {
    /* 等待UART就绪 */
    while ((UART_LSR & 0x20) == 0);
    UART_THR = c;
}

void puts(const char *s) {
    while (*s) {
        if (*s == '\n') {
            putchar('\r');
        }
        putchar(*s++);
    }
}

/* 简单的整数转字符串 */
static void print_num(uint64_t num, int base, int sign) {
    char buf[32];
    int i = 0;
    int is_neg = 0;

    if (sign && (int64_t)num < 0) {
        is_neg = 1;
        num = -(int64_t)num;
    }

    if (num == 0) {
        putchar('0');
        return;
    }

    while (num > 0) {
        int digit = num % base;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        num /= base;
    }

    if (is_neg) {
        putchar('-');
    }

    while (i > 0) {
        putchar(buf[--i]);
    }
}

void printk(const char *fmt, ...) {
    uint64_t *args = (uint64_t *)&fmt + 1;
    int arg_idx = 0;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    print_num(args[arg_idx++], 10, 1);
                    break;
                case 'u':
                    print_num(args[arg_idx++], 10, 0);
                    break;
                case 'x':
                    print_num(args[arg_idx++], 16, 0);
                    break;
                case 'p':
                    puts("0x");
                    print_num(args[arg_idx++], 16, 0);
                    break;
                case 's':
                    puts((const char *)args[arg_idx++]);
                    break;
                case 'c':
                    putchar((char)args[arg_idx++]);
                    break;
                case '%':
                    putchar('%');
                    break;
                default:
                    putchar('%');
                    putchar(*fmt);
                    break;
            }
        } else {
            if (*fmt == '\n') {
                putchar('\r');
            }
            putchar(*fmt);
        }
        fmt++;
    }
}
