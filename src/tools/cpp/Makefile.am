
bin_PROGRAMS = dump-binary-serialized

dump_binary_serialized_SOURCES = dump-binary-serialized.cpp
dump_binary_serialized_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
dump_binary_serialized_LDADD = ../../lib/cpp/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD) $(BOOST_PROGRAM_OPTIONS_LIB)

dump_binary_serialized_CXXFLAGS = -I../../lib/cpp $(thrift_CFLAGS) $(AM_CXXFLAGS)

