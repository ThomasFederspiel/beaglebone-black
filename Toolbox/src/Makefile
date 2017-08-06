MAKE_DIRS = $(subst /,,$(sort $(dir $(wildcard */))))

all: allexternal
#	@$(call make-clean,$(MAKE_DIRS))
	$(MAKE) -C util MAKEFLAGS= all
	$(MAKE) -C common MAKEFLAGS= all
	$(MAKE) -C concurrent MAKEFLAGS= all
	$(MAKE) -C container MAKEFLAGS= all
	$(MAKE) -C console MAKEFLAGS= all
	$(MAKE) -C exception MAKEFLAGS= all
	$(MAKE) -C logger MAKEFLAGS= all
	$(MAKE) -C prussdrv MAKEFLAGS= all
	$(MAKE) -C service MAKEFLAGS= all
	$(MAKE) -C boostasio MAKEFLAGS= all
	$(MAKE) -C protocol MAKEFLAGS= all
	
clean: cleanexternal
	$(MAKE) -C util MAKEFLAGS= clean
	$(MAKE) -C common MAKEFLAGS= clean
	$(MAKE) -C concurrent MAKEFLAGS= clean
	$(MAKE) -C container MAKEFLAGS= clean
	$(MAKE) -C console MAKEFLAGS= clean
	$(MAKE) -C exception MAKEFLAGS= clean
	$(MAKE) -C logger MAKEFLAGS= clean
	$(MAKE) -C prussdrv MAKEFLAGS= clean
	$(MAKE) -C service MAKEFLAGS= clean
	$(MAKE) -C boostasio MAKEFLAGS= clean
	$(MAKE) -C protocol MAKEFLAGS= clean

allexternal:

cleanexternal:
	
define make-clean
	$(foreach dir,$(1),call $(MAKE) -C ./$(dir) MAKEFLAGS= all;)
endef

.PHONY	: clean all allexternal cleanexternal
