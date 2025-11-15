#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <kernel/types.h>

/* 页大小和相关宏 */
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK (~(PAGE_SIZE - 1))

/* 页对齐 */
#define PAGE_ALIGN_UP(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)
#define PAGE_ALIGN_DOWN(addr) ((addr) & PAGE_MASK)

/* 页表项标志 (RISC-V Sv39) */
#define PTE_V (1UL << 0)  /* Valid */
#define PTE_R (1UL << 1)  /* Readable */
#define PTE_W (1UL << 2)  /* Writable */
#define PTE_X (1UL << 3)  /* Executable */
#define PTE_U (1UL << 4)  /* User */
#define PTE_G (1UL << 5)  /* Global */
#define PTE_A (1UL << 6)  /* Accessed */
#define PTE_D (1UL << 7)  /* Dirty */

/* 物理地址到页帧号 */
#define PA_TO_PFN(pa) ((pa) >> PAGE_SHIFT)
#define PFN_TO_PA(pfn) ((pfn) << PAGE_SHIFT)

/* 内存管理初始化 */
void mm_init(void);

/* 物理内存分配 */
void *alloc_page(void);
void free_page(void *page);

/* 虚拟内存管理 */
typedef uint64_t *pagetable_t;

pagetable_t create_pagetable(void);
void map_page(pagetable_t pt, uint64_t va, uint64_t pa, uint64_t flags);
void switch_pagetable(pagetable_t pt);

#endif
