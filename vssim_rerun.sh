# File: vssim_rerun.sh
# Date: 15-Sep-2017
# Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
# Copyright(c)2017
# Hanyang University, Seoul, Korea
# Embedded Software Systems Laboratory. All right reserved

#!/bin/bash

MNT="./RAMDISK/mnt"
QEMU_RUN_OPTION="-m 4096 -enable-kvm -vga cirrus -device nvme,drive=nvme1,serial=foo"
#QEMU_NETWORK_OPTION="-net nic,model=virtio -net user,hostfwd=tcp::37878-:22"
#-net bridge,br=br0
QEMU_IMG1="ssd_hda.img"
QEMU_IMG2="ssd_nvme.img"
QEMU_DIR="./QEMU/x86_64-softmmu"
OS_DIR="./OS"
OS_IMG="ubuntu-16.04.1-desktop-amd64.iso"
# Copy backup image
cp ./RAMDISK/${QEMU_IMG1} ${MNT}/${QEMU_IMG1}
cp ./RAMDISK/${QEMU_IMG2} ${MNT}/${QEMU_IMG2}
cp ./META_BK/*.dat ./META/

# Run VSSIM
sudo sysctl -w vm.drop_caches=1
# sudo ${QEMU_DIR}/qemu-system-x86_64 -hda ${MNT}/${QEMU_IMG1} -hdb ${MNT}/${QEMU_IMG2} -drive file=${MNT}/${QEMU_IMG2},if=none,id=nvme1 ${QEMU_RUN_OPTION} 
 sudo ${QEMU_DIR}/qemu-system-x86_64 -hda ${MNT}/${QEMU_IMG1} -drive file=${MNT}/${QEMU_IMG2},if=none,id=nvme1 ${QEMU_RUN_OPTION} -net nic -net user,hostfwd=tcp::37878-:22
# sudo ${QEMU_DIR}/qemu-system-x86_64 -hda ${MNT}/${QEMU_IMG1} -drive file=${MNT}/${QEMU_IMG2},if=none,id=nvme1 -boot d -cdrom ${OS_DIR}/${OS_IMG} ${QEMU_RUN_OPTION} -net nic -net user,hostfwd=tcp::37878-:22
