all::
	$(MAKE) -C lib gen-files
	$(MAKE) -C lib all test
	$(MAKE) -C plugins all
	$(MAKE) -C test gen-files
	$(MAKE) -C test all

clean::
	$(MAKE) -C lib clean
	$(MAKE) -C plugins clean
	$(MAKE) -C test clean

realclean:: clean
	$(MAKE) -C lib realclean
	$(MAKE) -C plugins realclean
	$(MAKE) -C test realclean

