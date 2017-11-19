AM_CFLAGS = -I../lib/cpp
bin_PROGRAMS = thrift-gen-typelib

thrift_gen_typelib_SOURCES = typelib_plugin.cpp ns_utils.cpp ns_utils.h
thrift_gen_typelib_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
thrift_gen_typelib_LDADD = ../../lib/cpp/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)

thrift_gen_typelib_CXXFLAGS = -I../../lib/cpp $(thrift_CFLAGS) $(AM_CXXFLAGS)

check_PROGRAMS = ns_utils_test

ns_utils_test_SOURCES = ns_utils_test.cpp ns_utils.cpp ns_utils.h
ns_utils_test_LDADD = $(thrift_LIBS)

TESTS = ns_utils_test
