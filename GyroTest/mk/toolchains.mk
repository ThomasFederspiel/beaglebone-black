ifdef WINDOWS 
	CROSS_TOOLCHAIN := C:\Packages\Beaglebone-black\host\gcc-linaro-7.3.1-2018.05-i686-mingw32_arm-linux-gnueabihf\bin
#    CROSS_TOOLCHAIN := C:\Packages\Beaglebone-black\host\gcc-linaro-4.8.2-2013.09-i686-mingw32_arm-linux-gnueabihf\bin
    CROSS_PREFIX := arm-linux-gnueabihf-
	SYSROOT := Z:\yocto-beaglebone-black-4.14-rt-sdk-gcc-7.3\sysroots\armv7at2hf-neon-poky-linux-gnueabi
#	SYSROOT := C:\Packages\Beaglebone-black\target\sysroot-3.8\cortexa8hf-vfp-neon-poky-linux-gnueabi
	PRU_TOOLCHAIN := C:/Packages/ti-cgt-pru_2.2.1
#	PRU_TOOLCHAIN := C:/Packages/ti-cgt-pru_2.1.5
#	PRU_TOOLCHAIN := C:/Packages/pru_2.0.0B2
	PRU_SSP = C:/Packages/pru-software-support-package
	PRU_ASSEMBLER_BIN := asmpru
	CSHELL := C:\Packages\cygwin-32\bin\bash.exe --login -c
	DTC_TOOLCHAIN :=
else
	$(error Linux not supported)
endif
