#ifndef _KERNEL_TYPES_H
#define _KERNEL_TYPES_H

/* 基础类型定义 */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;

typedef uint64_t size_t;
typedef int64_t  ssize_t;
typedef uint64_t uintptr_t;

/* 布尔类型 */
typedef enum {
    false = 0,
    true = 1
} bool;

/* NULL定义 */
#define NULL ((void *)0)

#endif
