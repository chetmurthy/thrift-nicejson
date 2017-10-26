
GENCPP0_test0 = \
gen-cpp/test0_constants.cpp \
gen-cpp/test0_constants.h \
gen-cpp/test0_types.cpp \
gen-cpp/test0_types.h

GENCPP_test = \
gen-cpp/test_constants.cpp \
gen-cpp/test_constants.h \
gen-cpp/test_types.cpp \
gen-cpp/test_types.h

GENCPP_TYPELIB_test = \
gen-cpp-typelib/test_typelib.cpp \
gen-cpp-typelib/test_typelib.h


GENCPP_TYPELIB_test0 = \
gen-cpp-typelib/test0_typelib.cpp \
gen-cpp-typelib/test0_typelib.h

GENCPP_TYPELIB_plugin = \
gen-cpp-typelib/plugin_typelib.cpp \
gen-cpp-typelib/plugin_typelib.h

BUILT_SOURCES = \
$(GENCPP0_test0) $(GENCPP_test) \
$(GENCPP_TYPELIB_test) \
$(GENCPP_TYPELIB_test0) \
$(GENCPP_TYPELIB_plugin)

CLEANFILES = $(BUILT_SOURCES)

check_PROGRAMS = test1

test1_SOURCES = test1.cpp $(BUILT_SOURCES)
test1_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
test1_LDADD = ../lib/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)

test1_CXXFLAGS = -I../lib $(thrift_CFLAGS) $(AM_CXXFLAGS)

TESTS= test1

THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift
PLUGIN_THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift

$(GENCPP0_test0):
	$(THRIFT) --gen cpp -r test0.thrift

$(GENCPP_test):
	$(THRIFT) --gen cpp -r test.thrift

$(GENCPP_TYPELIB_test):
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib test.thrift

$(GENCPP_TYPELIB_test0):
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib test0.thrift

$(GENCPP_TYPELIB_plugin):
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib $(PLUGIN_THRIFT)

