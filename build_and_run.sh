#!/bin/bash
# Get the directory where the script is located (the qemu root)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# 1. Build using ninja from the root
ninja -C "$DIR/build"

# 2. Sign the binary (assuming entitlements.plist is in the root)
codesign -s - --entitlements "$DIR/entitlements.plist" --force "$DIR/build/qemu-system-aarch64-unsigned"

"$DIR/build/qemu-system-aarch64-unsigned" \
  -M virt,highmem=on \
  -accel hvf \
  -cpu host \
  -m 4G \
  -smp 4 \
  -drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-aarch64-code.fd \
  -drive if=pflash,format=raw,file=./edk2-aarch64-vars.fd \
  -drive file=./ubuntu.qcow2,if=virtio,format=qcow2 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -device virtio-net-pci,netdev=net0 \
  -device virtio-custom-pci \
  -nographic

# 3. Launch the VM

#"$DIR/build/qemu-system-aarch64" \
#      -M virt,highmem=on \
#  -accel hvf \
#  -cpu host \
#  -m 4G \
#  -smp 4 \
#  -drive if=pflash,format=raw,readonly=on,file=/opt/homebrew/share/qemu/edk2-aarch64-code.fd \
#  -drive if=pflash,format=raw,file=./edk2-aarch64-vars.fd \
#  -drive file=ubuntu.qcow2,if=virtio,format=qcow2 \
#  -drive file=./ubuntu.iso,media=cdrom,readonly=on,file.locking=off \
#  -boot order=d \
#  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
#  -device virtio-net-pci,netdev=net0 \
#  -nographic

