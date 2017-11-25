
GENPY_tutorial = \
	gen-py/__init__.py \
	gen-py/test/__init__.py \
	gen-py/test/constants.py \
	gen-py/test/ttypes.py

GENTYPELIB_tutorial = \
	gen-typelib/shared.shared.binary_typelib \
	gen-typelib/shared.shared.typelib \
	gen-typelib/tutorial.tutorial.binary_typelib \
	gen-typelib/tutorial.tutorial.typelib

BUILT_SOURCES = $(GENPY_tutorial) $(GENTYPELIB_tutorial)

CLEANFILES = $(BUILT_SOURCES)

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
TUTORIAL_THRIFT=$(THRIFT_SRC_ROOT)/tutorial/tutorial.thrift

$(GENPY_tutorial): $(TUTORIAL_THRIFT)
	$(THRIFT) --gen py -r $^

$(GENTYPELIB_tutorial): $(TUTORIAL_THRIFT)
	PATH=../../plugins/cpp:${PATH} $(THRIFT) -r -gen typelib:typelib $^
