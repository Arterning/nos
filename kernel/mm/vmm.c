/* 虚拟内存管理 - 页表管理 */
#include <kernel/mm.h>
#include <kernel/printk.h>
#include <arch/riscv/riscv.h>

/* Sv39 页表结构 */
#define PGTABLE_ENTRIES 512
#define PTE_PPN_MASK 0x3FFFFFFFFFFC00UL

/* 获取虚拟地址的各级页表索引 */
#define VPN(va, level) (((va) >> (12 + 9 * (level))) & 0x1FF)

/* PTE操作 */
#define PTE_TO_PA(pte) (((pte) >> 10) << 12)
#define PA_TO_PTE(pa) (((pa) >> 12) << 10)

static pagetable_t kernel_pagetable;

/* 创建新页表 */
pagetable_t create_pagetable(void) {
    pagetable_t pt = (pagetable_t)alloc_page();
    if (!pt) {
        printk("[VMM] Failed to allocate page table\n");
        return NULL;
    }
    return pt;
}

/* 映射一个页 */
void map_page(pagetable_t pt, uint64_t va, uint64_t pa, uint64_t flags) {
    /* 遍历三级页表 */
    for (int level = 2; level > 0; level--) {
        uint64_t idx = VPN(va, level);
        uint64_t *pte = &pt[idx];

        if (*pte & PTE_V) {
            /* PTE有效，获取下一级页表 */
            pt = (pagetable_t)PTE_TO_PA(*pte);
        } else {
            /* PTE无效，分配新页表 */
            pagetable_t new_pt = create_pagetable();
            if (!new_pt) {
                return;
            }
            *pte = PA_TO_PTE((uint64_t)new_pt) | PTE_V;
            pt = new_pt;
        }
    }

    /* 设置叶子PTE */
    uint64_t idx = VPN(va, 0);
    pt[idx] = PA_TO_PTE(pa) | flags | PTE_V;
}

/* 切换页表 */
void switch_pagetable(pagetable_t pt) {
    uint64_t satp = SATP_MODE_SV39 | (((uint64_t)pt) >> PAGE_SHIFT);
    write_csr(satp, satp);
    sfence_vma();
}

/* 建立内核恒等映射 */
static void setup_kernel_mapping(void) {
    kernel_pagetable = create_pagetable();
    if (!kernel_pagetable) {
        printk("[VMM] Failed to create kernel page table\n");
        return;
    }

    /* 映射内核代码段 (0x80000000 - 0x88000000, 128MB) */
    uint64_t kernel_start = 0x80000000UL;
    uint64_t kernel_size = 128 * 1024 * 1024;  /* 128MB */

    for (uint64_t offset = 0; offset < kernel_size; offset += PAGE_SIZE) {
        uint64_t addr = kernel_start + offset;
        map_page(kernel_pagetable, addr, addr,
                 PTE_R | PTE_W | PTE_X | PTE_G);
    }

    /* 映射UART设备 (0x10000000) */
    map_page(kernel_pagetable, 0x10000000UL, 0x10000000UL,
             PTE_R | PTE_W | PTE_G);

    printk("  Kernel page table created\n");
}

void vmm_init(void) {
    setup_kernel_mapping();

    /* 暂时不切换页表，保持恒等映射 */
    /* switch_pagetable(kernel_pagetable); */

    printk("  Virtual memory ready (paging disabled for simplicity)\n");
}

void mm_init(void) {
    extern void pmm_init(void);
    extern void vmm_init(void);

    pmm_init();
    vmm_init();
}
