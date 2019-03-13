all:
	$(MAKE) -C prumanagerservice MAKEFLAGS= all
	$(MAKE) -C motorregulator MAKEFLAGS= all
	$(MAKE) -C telnetservice MAKEFLAGS= all
	$(MAKE) -C soundservice MAKEFLAGS= all
	$(MAKE) -C netmanagerservice MAKEFLAGS= all
	$(MAKE) -C streamservice MAKEFLAGS= all
	$(MAKE) -C mqttservice MAKEFLAGS= all
	$(MAKE) -C deadreckoningservice MAKEFLAGS= all
	
clean:
	$(MAKE) -C prumanagerservice MAKEFLAGS= clean
	$(MAKE) -C motorregulator MAKEFLAGS= clean
	$(MAKE) -C telnetservice MAKEFLAGS= clean
	$(MAKE) -C soundservice MAKEFLAGS= clean
	$(MAKE) -C netmanagerservice MAKEFLAGS= clean
	$(MAKE) -C streamservice MAKEFLAGS= clean
	$(MAKE) -C mqttservice MAKEFLAGS= clean
	$(MAKE) -C deadreckoningservice MAKEFLAGS= clean

.PHONY	: clean all
