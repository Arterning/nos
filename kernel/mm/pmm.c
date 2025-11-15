/* 物理内存管理器 - 简单的bitmap分配器 */
#include <kernel/mm.h>
#include <kernel/printk.h>
#include <kernel/string.h>

/* 内存布局 (QEMU RISC-V virt) */
#define KERNEL_BASE 0x80200000UL
#define MEMORY_SIZE (128 * 1024 * 1024)  /* 128MB */
#define MAX_PAGES (MEMORY_SIZE / PAGE_SIZE)

/* 外部符号 - 来自链接脚本 */
extern char kernel_end[];

/* 页分配位图 */
static uint8_t page_bitmap[MAX_PAGES / 8];
static uint64_t total_pages;
static uint64_t free_pages;
static uint64_t first_free_page;

void pmm_init(void) {
    /* 计算内核结束后的第一个可用页 */
    uint64_t kernel_end_addr = (uint64_t)kernel_end;
    first_free_page = PAGE_ALIGN_UP(kernel_end_addr - KERNEL_BASE) / PAGE_SIZE;

    total_pages = MAX_PAGES;
    free_pages = total_pages - first_free_page;

    /* 初始化位图 */
    memset(page_bitmap, 0, sizeof(page_bitmap));

    /* 标记内核占用的页为已分配 */
    for (uint64_t i = 0; i < first_free_page; i++) {
        page_bitmap[i / 8] |= (1 << (i % 8));
    }

    printk("  Physical memory: %u MB\n", MEMORY_SIZE / 1024 / 1024);
    printk("  Total pages: %u, Free pages: %u\n", total_pages, free_pages);
}

void *alloc_page(void) {
    /* 查找空闲页 */
    for (uint64_t i = first_free_page; i < total_pages; i++) {
        uint64_t byte_idx = i / 8;
        uint64_t bit_idx = i % 8;

        if (!(page_bitmap[byte_idx] & (1 << bit_idx))) {
            /* 找到空闲页 */
            page_bitmap[byte_idx] |= (1 << bit_idx);
            free_pages--;

            /* 计算物理地址 */
            uint64_t pa = KERNEL_BASE + i * PAGE_SIZE;

            /* 清零页面 */
            memset((void *)pa, 0, PAGE_SIZE);

            return (void *)pa;
        }
    }

    printk("[PMM] Out of memory!\n");
    return NULL;
}

void free_page(void *page) {
    uint64_t pa = (uint64_t)page;

    if (pa < KERNEL_BASE || pa >= KERNEL_BASE + MEMORY_SIZE) {
        printk("[PMM] Invalid page address: %p\n", page);
        return;
    }

    uint64_t page_idx = (pa - KERNEL_BASE) / PAGE_SIZE;
    uint64_t byte_idx = page_idx / 8;
    uint64_t bit_idx = page_idx % 8;

    if (!(page_bitmap[byte_idx] & (1 << bit_idx))) {
        printk("[PMM] Double free detected: %p\n", page);
        return;
    }

    page_bitmap[byte_idx] &= ~(1 << bit_idx);
    free_pages++;
}

uint64_t get_free_pages(void) {
    return free_pages;
}
