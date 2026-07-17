#!/bin/bash
# MT6575 Lenovo A690 3.0 kernel build runner.
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TARGET_PRODUCT=lcsh75_gb2
TOOLCHAIN_DIR="$SCRIPT_DIR/prebuilt/linux-x86/toolchain/arm-linux-androideabi-4.4.3"

export PATH=/usr/local/bin:/usr/bin:/bin:$TOOLCHAIN_DIR/bin
export TARGET_PRODUCT
export CROSS_COMPILE=arm-eabi-
export ARCH=arm

echo "==== ENV CHECK ===="
which arm-eabi-gcc || { echo "NO CROSS COMPILER"; exit 98; }
echo "Cross compiler: $(arm-eabi-gcc --version | head -1)"
echo "Source dir: $SCRIPT_DIR"
echo "TARGET_PRODUCT: $TARGET_PRODUCT"
echo "================"

cd "$SCRIPT_DIR/kernel" || exit 99
echo "Kernel dir: $(pwd)"

echo "==== SOURCE BUILD ENV ===="
source ../mediatek/build/shell.sh ../ kernel

echo "==== CLEAN ===="
rm -f .config .config.old
rm -f arch/arm/boot/zImage arch/arm/boot/Image "kernel_${TARGET_PRODUCT}.bin"
rm -rf "../mediatek/config/out/$TARGET_PRODUCT"
rm -rf "../mediatek/custom/out/$TARGET_PRODUCT"

echo "==== CONFIGURE ===="
make mediatek-configs

echo "==== BUILD KERNEL ===="
make -j8

echo "==== GENERATE MTK KERNEL BIN ===="
MKIMG="$SCRIPT_DIR/mediatek/build/tools/mkimage"
if [ ! -x "$MKIMG" ]; then chmod a+x "$MKIMG"; fi
"$MKIMG" arch/arm/boot/zImage KERNEL > "kernel_${TARGET_PRODUCT}.bin"
ls -la "kernel_${TARGET_PRODUCT}.bin" arch/arm/boot/zImage

echo "==== DONE ===="
