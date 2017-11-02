ifdef WINDOWS 
    CROSS_TOOLCHAIN := C:\Packages\Beaglebone-black\host\bin
    CROSS_PREFIX := arm-linux-gnueabihf-
	SYSROOT := Z:\yocto-adt-minimal\sysroots\cortexa8hf-vfp-neon-poky-linux-gnueabi
	PRU_TOOLCHAIN := C:/Packages/pru_2.0.0B2
#	PRU_TOOLCHAIN := C:/Packages/ti-cgt-pru_2.1.5
	PRU_SSP = C:/Packages/pru-software-support-package
	PRU_ASSEMBLER_BIN := asmpru
	CSHELL := C:\Packages\cygwin-32\bin\bash.exe --login -c
	DTC_TOOLCHAIN :=
else
	$(error Linux not supported)
endif
