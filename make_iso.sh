#!/bin/bash

mkdir -p isofiles/boot/grub
cp grub.cfg isofiles/boot/grub/
cp obj/kern/kernel isofiles/boot/myos.elf
grub-mkrescue -o os.iso isofiles/