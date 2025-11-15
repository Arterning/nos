#include <kernel/fs.h>
#include <kernel/string.h>
#include <kernel/printk.h>

/* 文件表 (简单的内存文件系统) */
static file_t file_table[MAX_FILES];

void fs_init(void) {
    /* 初始化文件表 */
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].in_use = false;
    }

    printk("  File system initialized (in-memory)\n");

    /* 创建一些示例文件 */
    fs_create("README.txt", FILE_TYPE_REGULAR);
    fs_write("README.txt",
             "Welcome to NOS - A Teaching Operating System!\n"
             "This is a simple RISC-V OS for educational purposes.\n",
             108);

    fs_create("info.txt", FILE_TYPE_REGULAR);
    fs_write("info.txt",
             "NOS supports:\n"
             "- Memory management\n"
             "- Process scheduling\n"
             "- Simple file system\n"
             "- Basic shell\n",
             95);
}

/* 查找文件 */
file_t *fs_find(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use &&
            strcmp(file_table[i].name, name) == 0) {
            return &file_table[i];
        }
    }
    return NULL;
}

/* 创建文件 */
int fs_create(const char *name, file_type_t type) {
    /* 检查文件是否已存在 */
    if (fs_find(name)) {
        printk("[FS] File already exists: %s\n", name);
        return -1;
    }

    /* 查找空闲槽 */
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].in_use) {
            file_table[i].in_use = true;
            file_table[i].type = type;
            file_table[i].size = 0;
            strcpy(file_table[i].name, name);
            memset(file_table[i].data, 0, MAX_FILESIZE);
            return 0;
        }
    }

    printk("[FS] No free file slots\n");
    return -1;
}

/* 删除文件 */
int fs_delete(const char *name) {
    file_t *file = fs_find(name);
    if (!file) {
        printk("[FS] File not found: %s\n", name);
        return -1;
    }

    file->in_use = false;
    return 0;
}

/* 写文件 */
int fs_write(const char *name, const void *buf, size_t size) {
    file_t *file = fs_find(name);
    if (!file) {
        printk("[FS] File not found: %s\n", name);
        return -1;
    }

    if (file->type != FILE_TYPE_REGULAR) {
        printk("[FS] Not a regular file: %s\n", name);
        return -1;
    }

    if (size > MAX_FILESIZE) {
        printk("[FS] File too large: %u bytes\n", size);
        return -1;
    }

    memcpy(file->data, buf, size);
    file->size = size;
    return size;
}

/* 读文件 */
int fs_read(const char *name, void *buf, size_t size) {
    file_t *file = fs_find(name);
    if (!file) {
        printk("[FS] File not found: %s\n", name);
        return -1;
    }

    if (file->type != FILE_TYPE_REGULAR) {
        printk("[FS] Not a regular file: %s\n", name);
        return -1;
    }

    size_t read_size = (size < file->size) ? size : file->size;
    memcpy(buf, file->data, read_size);
    return read_size;
}

/* 列出所有文件 */
void fs_list(void) {
    printk("Files:\n");
    printk("  %-20s %-10s %-10s\n", "Name", "Type", "Size");
    printk("  ----------------------------------------\n");

    int count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use) {
            const char *type_str = (file_table[i].type == FILE_TYPE_REGULAR)
                                   ? "file" : "dir";
            printk("  %-20s %-10s %-10u\n",
                   file_table[i].name, type_str, file_table[i].size);
            count++;
        }
    }

    printk("\n  Total: %d files\n", count);
}
