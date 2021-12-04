all:
	$(MAKE) -C user
	$(MAKE) -C kernel
	$(MAKE) -C evil
	$(MAKE) -C test