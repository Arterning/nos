#ifndef _KERNEL_PRINTK_H
#define _KERNEL_PRINTK_H

/* 内核打印函数 */
void printk(const char *fmt, ...);
void puts(const char *s);
void putchar(char c);

#endif
