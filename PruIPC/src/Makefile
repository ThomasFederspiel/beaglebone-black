all: allexternal
	$(MAKE) -C hostpruaccess MAKEFLAGS= all
	$(MAKE) -C prucore MAKEFLAGS= all
	$(MAKE) -C pruipc MAKEFLAGS= all
	$(MAKE) -C ipcdevices MAKEFLAGS= all
	$(MAKE) -C ipcservicecore MAKEFLAGS= all
	
clean: cleanexternal
	$(MAKE) -C hostpruaccess MAKEFLAGS= clean
	$(MAKE) -C prucore MAKEFLAGS= clean
	$(MAKE) -C pruipc MAKEFLAGS= clean
	$(MAKE) -C ipcdevices MAKEFLAGS= clean
	$(MAKE) -C ipcservicecore MAKEFLAGS= clean

allexternal:

cleanexternal:
	
.PHONY	: clean all allexternal cleanexternal
