
GENCPP_tutorial = \
gen-cpp/Calculator.cpp \
gen-cpp/Calculator.h \
gen-cpp/SharedService.cpp \
gen-cpp/SharedService.h \
gen-cpp/shared_constants.cpp \
gen-cpp/shared_constants.h \
gen-cpp/shared_types.cpp \
gen-cpp/shared_types.h \
gen-cpp/tutorial_constants.cpp \
gen-cpp/tutorial_constants.h \
gen-cpp/tutorial_types.cpp \
gen-cpp/tutorial_types.h

GENCPP_TYPELIB_tutorial = \
gen-cpp-typelib/tutorial_typelib.cpp \
gen-cpp-typelib/tutorial_typelib.h \
gen-cpp-typelib/shared_typelib.cpp \
gen-cpp-typelib/shared_typelib.h

GENJSON_tutorial = gen-json/tutorial.json

BUILT_SOURCES = \
 $(GENCPP_tutorial) \
$(GENCPP_TYPELIB_tutorial) \
$(GENJSON_tutorial)

CLEANFILES = $(BUILT_SOURCES)

bin_PROGRAMS = CppServer CppClient

CppClient_SOURCES = CppClient.cpp $(BUILT_SOURCES)
CppClient_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
CppClient_LDADD = ../../lib/cpp/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)
CppClient_CXXFLAGS = -I../../lib/cpp $(thrift_CFLAGS) $(AM_CXXFLAGS)

CppServer_SOURCES = CppServer.cpp $(BUILT_SOURCES)
CppServer_LDFLAGS = -Wl,-rpath -Wl,$(thrift_LIBDIR)
CppServer_LDADD = ../../lib/cpp/libnicejson.a $(thrift_LIBS) $(BOOST_FILESYSTEM_LDADD) $(BOOST_SYSTEM_LDADD)
CppServer_CXXFLAGS = -I../../lib/cpp $(thrift_CFLAGS) $(AM_CXXFLAGS)

THRIFT=$(thrift_EXEC_PREFIX)/bin/thrift
TUTORIAL_THRIFT=$(THRIFT_SRC_ROOT)/tutorial/tutorial.thrift

$(GENCPP_tutorial): $(TUTORIAL_THRIFT)
	$(THRIFT) --gen cpp -r $^

$(GENCPP_TYPELIB_tutorial): $(TUTORIAL_THRIFT)
	PATH=../../plugins/cpp:${PATH} $(THRIFT) -r -gen typelib:cpp,typelib $^

$(GENJSON_tutorial):  $(TUTORIAL_THRIFT)
	$(THRIFT) --gen json -r $^
