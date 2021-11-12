#!/bin/bash

set -xe

sudo dmesg -C
sudo insmod mycall.ko
./test.o
dmesg
sudo dmesg -C
./bench.o
dmesg
sudo rmmod mycall