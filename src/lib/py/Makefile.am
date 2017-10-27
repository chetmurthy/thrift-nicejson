PYLIBPATH = $(shell python-config --exec-prefix)/lib
LIB = -L$(PYLIBPATH) $(shell python-config --libs) -lboost_python
OPTS = $(shell python-config --includes) -O2 -fPIC

lib_LTLIBRARIES = libzoo.la
libzoo_la_SOURCES = zoo.cpp
libzoo_la_LDFLAGS = $(LIB) -Wl,-rpath,$(PYLIBPATH)
libzoo_la_CXXFLAGS = $(OPTS)

