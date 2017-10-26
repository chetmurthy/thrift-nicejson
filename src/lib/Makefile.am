GENCPP = gen-cpp/plugin_constants.cpp \
  gen-cpp/plugin_constants.h \
  gen-cpp/plugin_types.h \
  gen-cpp/plugin_types.cpp

BUILT_SOURCES = $(GENCPP)
CLEANFILES = $(GENCPP)
noinst_LIBRARIES=libnicejson.a
libnicejson_a_SOURCES= NiceJSON.cpp NiceJSON.h ns_utils.cpp ns_utils.h $(GENCPP)


libnicejson_a_CXXFLAGS = $(thrift_CFLAGS) $(AM_CXXFLAGS)
#libnicejson_a_LIBADD    = $(thrift_LIBS)

check_PROGRAMS = jsontest ns_utils_test

jsontest_SOURCES = jsontest.cpp

ns_utils_test_SOURCES = ns_utils_test.cpp
ns_utils_test_LDADD = libnicejson.a $(thrift_LIBS)

TESTS = jsontest ns_utils_test

THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift
PLUGIN_THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift

$(GENCPP): 
	$(THRIFT) --gen cpp -r $(PLUGIN_THRIFT)
