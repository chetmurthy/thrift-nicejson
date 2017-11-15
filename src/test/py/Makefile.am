
GENPY_test = \
	gen-py/__init__.py \
	gen-py/test/__init__.py \
	gen-py/test/constants.py \
	gen-py/test/ttypes.py

BUILT_SOURCES = $(GENPY_test)

CLEANFILES = $(BUILT_SOURCES)

check-local: all
	$(PYTHON) test1.py

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift

$(GENPY_test):
	$(THRIFT) --gen py -r ../cpp/test.thrift
