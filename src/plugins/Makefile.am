AM_CFLAGS = -I../lib
bin_PROGRAMS = thrift-gen-typelib

thrift_gen_typelib_SOURCES = typelib_plugin.cpp
thrift_gen_typelib_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
thrift_gen_typelib_LDADD = ../lib/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)

thrift_gen_typelib_CXXFLAGS = -I../lib $(thrift_CFLAGS) $(AM_CXXFLAGS)
