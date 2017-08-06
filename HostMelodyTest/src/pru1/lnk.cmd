/******************************************************************************/
/*  AM3359_PRU.cmd                                                            */
/*    Description: This file is a sample linker command file that can be      */
/*    used for linking programs built with the PRU C compiler and running the */
/*    resulting .out file on an AM3359 PRU0. Use it as a guideline.  You will */
/*    want to change the memory layout to match your specific target system.  */
/*    You may want to change the allocation scheme according to the size of   */
/*    your program.                                                           */
/******************************************************************************/

-cr
-stack 0x100
-heap 0x100

MEMORY
{
    PAGE 0:
      PRUIMEM:   org = 0x00000000 len = 0x00002000  /* 8kB PRU0 Instruction RAM */

    PAGE 1:
      PRUDMEM_CONFIG:   org = 0x00000000 len = 0x000000FF /* 255 bytes PRU Data RAM 0 */
      PRUDMEM:   org = 0x00000100 len = 0x00001F00 /* 8kB PRU Data RAM 0 */
   	  SHAREDMEM: org = 0x00010000 len = 0x00003000  /* 12kB Shared RAM */
	  PRU0_CTRL: org = 0x00022000 len = 0x00000030
	  PRU1_CTRL: org = 0x00024000 len = 0x00000030
	  
	PAGE 2:
	  /* Constant Table Memory directives. */

	  INTC: org = 0x00020000 len = 0x00001504	CREGISTER=0
	  DMTIMER2: org = 0x48040000 len = 0x00000100	CREGISTER=1
	  I2C1: org = 0x4802A000 len = 0x00000100	CREGISTER=2
	  ECAP: org = 0x00030000 len = 0x00000100	CREGISTER=3
	  CFG: org = 0x00026000 len = 0x00000100	CREGISTER=4
	  MMCHS0: org = 0x48060000 len = 0x00000100	CREGISTER=5
	  MCSPI0: org = 0x48030000 len = 0x00000100	CREGISTER=6
	  UART0: org = 0x00028000 len = 0x00000100	CREGISTER=7
	  MCASP0_DMA: org = 0x46000000 len = 0x00000100	CREGISTER=8
	  GEMAC: org = 0x4A100000 len = 0x00000100	CREGISTER=9
	  RSVD10: org = 0x48318000 len = 0x00000100	CREGISTER=10
	  UART1: org = 0x48022000 len = 0x00000100	CREGISTER=11
	  UART2: org = 0x48024000 len = 0x00000100	CREGISTER=12
	  RSVD13: org = 0x48310000 len = 0x00000100	CREGISTER=13
	  DCAN0: org = 0x481CC000 len = 0x00000100	CREGISTER=14
	  DCAN1: org = 0x481D0000 len = 0x00000100	CREGISTER=15
	  MCSPI1: org = 0x481A0000 len = 0x00000100	CREGISTER=16
	  I2C2: org = 0x4819C000 len = 0x00000100	CREGISTER=17
	  EHRPWM1: org = 0x48300000 len = 0x00000260	CREGISTER=18
	  EHRPWM2: org = 0x48302000 len = 0x00000260	CREGISTER=19
	  EHRPWM3: org = 0x48304000 len = 0x00000260	CREGISTER=20
	  MDIO: org = 0x00032400 len = 0x00000100	CREGISTER=21
	  MBX0: org = 0x480C8000 len = 0x00000100	CREGISTER=22
	  SPINLOCK: org = 0x480CA000 len = 0x00000100	CREGISTER=23
/*	  PRU0_1: org = 0x00000000 len = 0x00000100	CREGISTER=24
 	  PRU1_0: org = 0x00000000 len = 0x00000100	CREGISTER=25 */
	  IEP: org = 0x0002E000 len = 0x0000031C	CREGISTER=26
/*	  MII_RT: org = 0x00032000 len = 0x00000100	CREGISTER=27 */
	  SHRAM: org = 0x00000000 len = 0x00000100	CREGISTER=28 
	  TPCC: org = 0x49000000 len = 0x000010A0	CREGISTER=29
	  L3OCMC: org = 0x40000000 len = 0x00010000	CREGISTER=30
	  DDR: org = 0x80000000 len = 0x00000100	CREGISTER=31
	  PER: org = 0x44E00000 len = 0x00000340	CREGISTER=32
}

SECTIONS
{
    /* Thanks to Jakub for the next line! */
    .text:_c_int00* >  0x0, PAGE 0
    .PRU_CONFIG     >  PRUDMEM_CONFIG, PAGE 1
    .stack          >  PRUDMEM, PAGE 1
    .bss            >  PRUDMEM, PAGE 1
    .cio            >  PRUDMEM, PAGE 1
    .const          >  PRUDMEM, PAGE 1
    .data           >  PRUDMEM, PAGE 1
    .switch         >  PRUDMEM, PAGE 1
    .sysmem         >  PRUDMEM, PAGE 1
    .cinit          >  PRUDMEM, PAGE 1
	.rodata		    >  PRUDMEM, PAGE 1
	.rofardata	    >  PRUDMEM, PAGE 1
	.farbss		    >  PRUDMEM, PAGE 1
	.fardata	    >  PRUDMEM, PAGE 1
	.PRU0_CTRL	    >  PRU0_CTRL, PAGE 1
	.PRU1_CTRL	    >  PRU1_CTRL, PAGE 1
}