MK_DIR := $(PWD)/../mk

#SILENT := 1
#DEBUG := 1
LTTNG := 1

export MK_DIR
export LTTNG

all: allexternal
	$(MAKE) -C core MAKEFLAGS= all
	$(MAKE) -C main MAKEFLAGS= all
	$(MAKE) -C pru0 MAKEFLAGS= all
	
clean: cleanexternal
	$(MAKE) -C core MAKEFLAGS= clean
	$(MAKE) -C main MAKEFLAGS= clean
	$(MAKE) -C pru0 MAKEFLAGS= clean

allexternal:
	$(MAKE) -C ../../Toolbox/src MAKEFLAGS= all
	$(MAKE) -C ../../PruIPC/src EXPORT_INCLUDE=MotorAppl/src/export MAKEFLAGS= all
	$(MAKE) -C ../../CommonServices/src EXPORT_INCLUDE=MotorAppl/src/export MAKEFLAGS= all

cleanexternal:
	$(MAKE) -C ../../Toolbox/src MAKEFLAGS= clean
	$(MAKE) -C ../../PruIPC/src MAKEFLAGS= clean
	$(MAKE) -C ../../CommonServices/src MAKEFLAGS= clean
	
.PHONY	: clean all allexternal cleanexternal
