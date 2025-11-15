# NOS Makefile

# 工具链配置
CROSS_COMPILE ?= riscv64-unknown-elf-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# 编译选项
CFLAGS = -Wall -Wextra -O2 -ffreestanding -nostdlib -nostdinc
CFLAGS += -mcmodel=medany -march=rv64imac_zicsr -mabi=lp64
CFLAGS += -I./include

LDFLAGS = -nostdlib
LDSCRIPT = boot/linker.ld

# 源文件
C_SOURCES = $(wildcard kernel/*.c) \
            $(wildcard kernel/arch/riscv/*.c) \
            $(wildcard kernel/mm/*.c) \
            $(wildcard kernel/process/*.c) \
            $(wildcard kernel/fs/*.c) \
            $(wildcard kernel/drivers/*.c) \
            $(wildcard lib/*.c)

ASM_SOURCES = $(wildcard boot/*.S) \
              $(wildcard kernel/arch/riscv/*.S)

# 目标文件
C_OBJS = $(C_SOURCES:.c=.o)
ASM_OBJS = $(ASM_SOURCES:.S=.o)
OBJS = $(ASM_OBJS) $(C_OBJS)

# 输出
TARGET = nos.elf
BINARY = nos.bin

.PHONY: all clean run debug

all: $(BINARY)

# 编译C文件
%.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 编译汇编文件
%.o: %.S
	@echo "AS $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# 链接
$(TARGET): $(OBJS)
	@echo "LD $@"
	@$(LD) $(LDFLAGS) -T $(LDSCRIPT) $(OBJS) -o $@

# 生成二进制文件
$(BINARY): $(TARGET)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@
	@echo "Build complete: $@"

# 清理
clean:
	@echo "Cleaning..."
	@rm -f $(OBJS) $(TARGET) $(BINARY)
	@echo "Clean complete"

# 在QEMU中运行
run: $(BINARY)
	@echo "Starting QEMU..."
	qemu-system-riscv64 -machine virt -bios none \
		-kernel $(TARGET) -nographic

# 调试模式
debug: $(BINARY)
	@echo "Starting QEMU in debug mode..."
	qemu-system-riscv64 -machine virt -bios none \
		-kernel $(TARGET) -nographic -s -S

# 显示帮助
help:
	@echo "NOS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all    - Build the kernel (default)"
	@echo "  clean  - Remove build artifacts"
	@echo "  run    - Run kernel in QEMU"
	@echo "  debug  - Run kernel in QEMU with GDB server"
	@echo "  help   - Show this help message"
