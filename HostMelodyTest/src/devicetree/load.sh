#!/bin/sh
echo "Before ------------------------------"
cat /sys/devices/bone_capemgr.9/slots
echo pru_enable > /sys/devices/bone_capemgr.9/slots
#echo pru_pwm > /sys/devices/bone_capemgr.9/slots
#echo pru_ecap > /sys/devices/bone_capemgr.9/slots
#echo pru_gpio > /sys/devices/bone_capemgr.9/slots
echo pru_gpio_all > /sys/devices/bone_capemgr.9/slots

echo "After ------------------------------"
cat /sys/devices/bone_capemgr.9/slots

