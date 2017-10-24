
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift
PLUGIN_THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift

all: gen-files thrift-gen-cpp-typelib

test: test1 jsontest ns_utils_test
	./test1
	./jsontest
	./ns_utils_test

gen-files:: gen-cpp gen-json

gen-cpp: test.thrift $(PLUGIN_THRIFT)
	$(THRIFT) --gen cpp -r $(PLUGIN_THRIFT)
	$(THRIFT) --gen cpp -r test.thrift

gen-cpp-typelib: test.thrift $(PLUGIN_THRIFT) thrift-gen-cpp-typelib
	PATH=.:${PATH} thrift -r -gen cpp-typelib test.thrift
	PATH=.:${PATH} thrift -r -gen cpp-typelib $(PLUGIN_THRIFT)

gen-json: test.thrift $(PLUGIN_THRIFT)
	$(THRIFT) --gen json -r $(PLUGIN_THRIFT)
	$(THRIFT) --gen json -r test.thrift

TESTSRC=$(wildcard gen-cpp/test*.cpp)
PLUGINSRC=$(wildcard gen-cpp/plugin*.cpp)

TESTOBJ=$(patsubst %.cpp,%.o,$(TESTSRC))
PLUGINOBJ=$(patsubst %.cpp,%.o,$(PLUGINSRC))
GENCPPOBJ=$(TESTOBJ) $(PLUGINOBJ)


LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib -lboost_filesystem -lboost_system

INCLUDES=-I$(THRIFTROOT)/include -I.  -Igen-cpp -I../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

test1: test1.o NiceJSON.o $(GENCPPOBJ) gen-cpp-typelib/test_typelib.o gen-cpp-typelib/plugin_typelib.o
	g++  $(CPPFLAGS) -o test1 $^ -lthriftc $(LINKFLAGS)

jsontest: jsontest.o NiceJSON.o
	g++  $(CPPFLAGS) -o jsontest $^ -lthriftc $(LINKFLAGS)

ns_utils_test: ns_utils_test.o ns_utils.o
	g++  $(CPPFLAGS) -o ns_utils_test $^ $(LINKFLAGS)

thrift-gen-cpp-typelib: cpp_typelib_plugin.o ns_utils.o $(PLUGINOBJ)
	g++ $(CPPFLAGS) -o thrift-gen-cpp-typelib $^ -lthriftc $(LINKFLAGS)

gen-cpp/%.o: gen-cpp/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

gen-cpp-typelib/%.o: gen-cpp-typelib/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o ThriftThrift test1 thrift-gen-cpp-typelib jsontest ns_utils_test

realclean:: clean
	rm -rf gen-*

NiceJSON.o: NiceJSON.h

foo: foo.o
	g++  $(CPPFLAGS) -o foo $^ -lthriftc $(LINKFLAGS)
