GENCPP = gen-cpp/plugin_constants.cpp \
  gen-cpp/plugin_constants.h \
  gen-cpp/plugin_types.h \
  gen-cpp/plugin_types.cpp

BUILT_SOURCES = $(GENCPP)
CLEANFILES = $(GENCPP)
lib_LIBRARIES=libnicejson.a
libnicejson_a_SOURCES= NiceJSON.cpp NiceJSON.h TNiceJSONProtocol.h TNiceJSONProtocol.cpp $(GENCPP)

pkgincludedir = $(includedir)/thrift-nicejson
pkginclude_HEADERS = json.hpp NiceJSON.h

libnicejson_a_CXXFLAGS = -fPIC $(thrift_CFLAGS) $(AM_CXXFLAGS)
#libnicejson_a_LIBADD    = $(thrift_LIBS)

check_PROGRAMS = jsontest iatest

jsontest_SOURCES = jsontest.cpp

iatest_SOURCES = iatest.cpp
iatest_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
iatest_LDADD = ../../lib/cpp/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)
iatest_CXXFLAGS = -I../../lib/cpp $(thrift_CFLAGS) $(AM_CXXFLAGS)

TESTS = jsontest iatest

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
PLUGIN_THRIFT=$(THRIFT_SRC_ROOT)/compiler/cpp/src/thrift/plugin/plugin.thrift

$(GENCPP): 
	$(THRIFT) --gen cpp -r $(PLUGIN_THRIFT)
