#ifndef _KERNEL_FS_H
#define _KERNEL_FS_H

#include <kernel/types.h>

/* 文件类型 */
typedef enum {
    FILE_TYPE_REGULAR = 0,
    FILE_TYPE_DIRECTORY
} file_type_t;

/* 文件描述符 */
#define MAX_FILES 32
#define MAX_FILENAME 64
#define MAX_FILESIZE (64 * 1024)  /* 64KB */

typedef struct {
    char name[MAX_FILENAME];
    file_type_t type;
    size_t size;
    uint8_t data[MAX_FILESIZE];
    bool in_use;
} file_t;

/* 文件系统函数 */
void fs_init(void);
int fs_create(const char *name, file_type_t type);
int fs_delete(const char *name);
int fs_write(const char *name, const void *buf, size_t size);
int fs_read(const char *name, void *buf, size_t size);
void fs_list(void);
file_t *fs_find(const char *name);

#endif
