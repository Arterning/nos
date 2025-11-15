# NOS - 教学操作系统 (RISC-V版)

NOS (NingOS) 是一个用于教学的简单操作系统，运行在RISC-V架构上。它包含了现代操作系统的核心概念，适合学习操作系统原理。

## 特性

- **RISC-V架构支持**: 基于RISC-V 64位架构
- **内存管理**:
  - 物理内存分配器 (Bitmap分配)
  - 虚拟内存管理 (Sv39分页机制)
- **进程调度**:
  - 进程控制块 (PCB)
  - 时间片轮转调度
  - 上下文切换
- **中断处理**:
  - 中断向量表
  - 异常处理
  - 时钟中断
- **文件系统**:
  - 简单的内存文件系统
  - 支持文件创建、读写、删除
- **Shell命令行**:
  - 交互式命令行界面
  - 多种内置命令

## 系统要求

- RISC-V交叉编译工具链 (`riscv64-unknown-elf-gcc`)
- QEMU RISC-V模拟器 (`qemu-system-riscv64`)
- Make构建工具

### 安装工具链 (Ubuntu/Debian)

```bash
# 安装RISC-V工具链
sudo apt-get install gcc-riscv64-unknown-elf

# 安装QEMU
sudo apt-get install qemu-system-misc
```

## 构建和运行

### 编译

```bash
make
```

### 运行

```bash
make run
```

或使用提供的脚本：

```bash
chmod +x run.sh
./run.sh
```

### 退出QEMU

按 `Ctrl-A` 然后按 `X`

## 项目结构

```
nos/
├── boot/              # 引导代码
│   ├── start.S        # RISC-V启动汇编
│   └── linker.ld      # 链接脚本
├── kernel/            # 内核代码
│   ├── main.c         # 内核主函数
│   ├── arch/          # 架构相关代码
│   │   └── riscv/     # RISC-V相关实现
│   ├── mm/            # 内存管理
│   │   ├── pmm.c      # 物理内存管理
│   │   └── vmm.c      # 虚拟内存管理
│   ├── process/       # 进程管理
│   │   └── process.c  # 进程调度器
│   ├── fs/            # 文件系统
│   │   └── fs.c       # 简单文件系统
│   └── drivers/       # 驱动程序
│       └── shell.c    # Shell命令行
├── lib/               # 库函数
│   ├── string.c       # 字符串函数
│   └── printk.c       # 内核打印函数
├── include/           # 头文件
├── Makefile           # 构建文件
└── README.md          # 本文件
```

## Shell命令

NOS提供了一个简单的Shell，支持以下命令：

| 命令 | 说明 | 示例 |
|------|------|------|
| `help` | 显示帮助信息 | `help` |
| `ls` | 列出文件 | `ls` |
| `cat <file>` | 显示文件内容 | `cat README.txt` |
| `ps` | 列出进程 | `ps` |
| `mem` | 显示内存信息 | `mem` |
| `echo <msg>` | 打印消息 | `echo Hello World` |
| `clear` | 清屏 | `clear` |
| `about` | 关于NOS | `about` |

## 教学要点

### 1. 启动流程
- `boot/start.S`: RISC-V启动代码，设置栈、清空BSS段
- `kernel/main.c`: 内核初始化流程

### 2. 内存管理
- **物理内存**: 简单的bitmap分配器 (`kernel/mm/pmm.c`)
- **虚拟内存**: Sv39三级页表 (`kernel/mm/vmm.c`)

### 3. 中断处理
- 中断向量表: `kernel/arch/riscv/trap.S`
- 中断处理: `kernel/arch/riscv/trap.c`

### 4. 进程调度
- PCB结构: `include/kernel/process.h`
- 调度器: `kernel/process/process.c`
- 上下文切换: `kernel/arch/riscv/switch.S`

### 5. 文件系统
- 内存文件系统: `kernel/fs/fs.c`
- 支持基本的文件操作

## 学习建议

1. **阅读代码顺序**:
   - 从`boot/start.S`开始，了解启动流程
   - 然后阅读`kernel/main.c`，理解初始化过程
   - 深入各个子系统（内存、进程、文件系统）

2. **实验任务**:
   - 修改调度算法（如优先级调度）
   - 添加新的Shell命令
   - 实现系统调用机制
   - 添加更多的文件系统功能

3. **扩展方向**:
   - 实现用户态进程
   - 添加更多驱动（网络、存储）
   - 实现真正的文件系统（如FAT）
   - 添加多核支持

## 调试

使用GDB调试：

```bash
# 终端1: 启动QEMU并等待GDB连接
make debug

# 终端2: 启动GDB
riscv64-unknown-elf-gdb nos.elf
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

## 参考资料

- [RISC-V规范](https://riscv.org/technical/specifications/)
- [xv6: 一个简单的Unix-like教学操作系统](https://pdos.csail.mit.edu/6.828/2020/xv6.html)
- [QEMU文档](https://www.qemu.org/docs/master/)

## 许可证

本项目仅用于教学目的。

## 常见问题

**Q: 如何修改内存大小？**
A: 修改`kernel/mm/pmm.c`中的`MEMORY_SIZE`常量，以及Makefile中QEMU的`-m`参数。

**Q: 如何添加新的系统调用？**
A: 在`kernel/arch/riscv/trap.c`中添加系统调用处理代码。

**Q: 为什么没有看到进程切换？**
A: 当前实现中时钟中断未完全配置，进程需要手动调用`yield()`来切换。

## 贡献

欢迎提交问题和改进建议！
