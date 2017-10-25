all::
	$(MAKE) -C src/lib gen-files
	$(MAKE) -C src/lib all test
	$(MAKE) -C src/plugins all
	$(MAKE) -C src/test gen-files
	$(MAKE) -C src/test all

clean::
	$(MAKE) -C src/lib clean
	$(MAKE) -C src/plugins clean
	$(MAKE) -C src/test clean

realclean:: clean
	$(MAKE) -C src/lib realclean
	$(MAKE) -C src/plugins realclean
	$(MAKE) -C src/test realclean

