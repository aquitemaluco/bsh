
all: force_look
	cd src; $(MAKE) 

clean: force_look
	cd src; $(MAKE) clean

force_look:
	true

