#!/bin/sh

# Start OpenAMP
modprobe rpmsg_user_dev_driver
echo fw_bm_mandelbrot.elf > /sys/class/remoteproc/remoteproc0/firmware
echo start > /sys/class/remoteproc/remoteproc0/state

# start server daemon
cd /home/root/www/MandelbrotServer/
python main.py
