cd kernel
source ~/osbook/devenv/buildenv.sh
make
cd ~/edk2
build
~/osbook/devenv/run_qemu.sh ~/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi $OS_DIR/kernel/kernel.elf
cd $OS_DIR