#!/bin/bash
# NOS启动脚本

echo "Building NOS..."
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful! Starting QEMU..."
    echo "Press Ctrl-A then X to exit QEMU"
    echo ""
    make run
else
    echo "Build failed!"
    exit 1
fi
