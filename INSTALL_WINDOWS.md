# Windows环境安装指南

## 方法1: 使用WSL2 (推荐)

### 1. 安装WSL2

```powershell
# 在PowerShell（管理员）中运行
wsl --install
```

### 2. 在WSL2中安装工具链

```bash
# 更新包列表
sudo apt update

# 安装RISC-V工具链
sudo apt install gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf

# 安装QEMU
sudo apt install qemu-system-misc

# 安装make
sudo apt install build-essential
```

### 3. 编译和运行

```bash
# 进入项目目录
cd /mnt/c/Users/ningh/Desktop/coding/nos

# 编译
make

# 运行
make run
```

## 方法2: 使用预编译工具链

### 1. 下载RISC-V工具链

从以下位置下载预编译的RISC-V工具链：
- https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases

下载Windows版本，例如：
`xpack-riscv-none-elf-gcc-13.2.0-2-win32-x64.zip`

### 2. 解压并配置环境变量

1. 解压到 `C:\riscv`
2. 添加 `C:\riscv\bin` 到系统PATH

### 3. 安装QEMU for Windows

从 https://qemu.weidai.com.cn/download/qemu-w64-setup-20240903.exe 下载并安装

### 4. 修改Makefile

将Makefile中的：
```makefile
CROSS_COMPILE ?= riscv64-unknown-elf-
```
改为：
```makefile
CROSS_COMPILE ?= riscv-none-elf-
```

## 方法3: 使用Docker

### 1. 安装Docker Desktop for Windows

从 https://www.docker.com/products/docker-desktop 下载并安装

### 2. 创建Dockerfile

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc-riscv64-unknown-elf \
    binutils-riscv64-unknown-elf \
    qemu-system-misc \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /nos
```

### 3. 构建和运行

```powershell
# 构建Docker镜像
docker build -t nos-build .

# 运行编译
docker run -v ${PWD}:/nos nos-build make

# 运行OS
docker run -it -v ${PWD}:/nos nos-build make run
```

## 验证安装

检查工具是否正确安装：

```bash
# 检查编译器
riscv64-unknown-elf-gcc --version

# 检查QEMU
qemu-system-riscv64 --version

# 检查make
make --version
```

## 常见问题

### Q: WSL2中找不到RISC-V工具链
A: 使用以下命令从源码安装：
```bash
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv64imac --with-abi=lp64
sudo make
```

### Q: QEMU启动失败
A: 确保使用 `qemu-system-riscv64` 而不是 `qemu-riscv64`

### Q: 权限错误
A: 在WSL中，确保文件有执行权限：
```bash
chmod +x run.sh
```
