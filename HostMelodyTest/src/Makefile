all: allexternal
	$(MAKE) -C core MAKEFLAGS= all
	$(MAKE) -C host MAKEFLAGS= all
	$(MAKE) -C pru0 MAKEFLAGS= all
	$(MAKE) -C pru1 MAKEFLAGS= all
	
clean: cleanexternal
	$(MAKE) -C core MAKEFLAGS= clean
	$(MAKE) -C host MAKEFLAGS= clean
	$(MAKE) -C pru0 MAKEFLAGS= clean
	$(MAKE) -C pru1 MAKEFLAGS= clean

allexternal:
	$(MAKE) -C ../../Toolbox/src MAKEFLAGS= all
	$(MAKE) -C ../../PruIPC/src EXPORT_INCLUDE=HostMelodyTest/src/export MAKEFLAGS= all
	$(MAKE) -C ../../CommonServices/src EXPORT_INCLUDE=HostMelodyTest/src/export MAKEFLAGS= all

cleanexternal:
	$(MAKE) -C ../../Toolbox/src MAKEFLAGS= clean
	$(MAKE) -C ../../PruIPC/src MAKEFLAGS= clean
	$(MAKE) -C ../../CommonServices/src MAKEFLAGS= clean
	
.PHONY	: clean all allexternal cleanexternal
