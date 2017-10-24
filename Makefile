
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift

all: gen-files thrift-gen-wirejson thrift-gen-cpp-typelib

test: test1 jsontest test.wirejson plugin.wirejson
	./test1
	./jsontest

plugin.wirejson: $(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift thrift-gen-wirejson
	PATH=.:${PATH} thrift -r -gen wirejson $< > $@

gen-files:: *.thrift gen-cpp gen-json

gen-cpp: test.thrift
	$(THRIFT) --gen cpp -r $(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift
	$(THRIFT) --gen cpp -r test.thrift

gen-json: test.thrift
	$(THRIFT) --gen json -r test.thrift

gen-cpp-typelib: test.thrift thrift-gen-cpp-typelib
	PATH=.:${PATH} thrift -r -gen cpp-typelib $<

gen-cpp-typelib/%.cpp: gen-cpp-typelib

TESTSRC=$(wildcard gen-cpp/test*.cpp)
PLUGINSRC=$(wildcard gen-cpp/plugin*.cpp)

TESTOBJ=$(patsubst %.cpp,%.o,$(TESTSRC))
PLUGINOBJ=$(patsubst %.cpp,%.o,$(PLUGINSRC))
GENCPPOBJ=$(TESTOBJ) $(PLUGINOBJ)


LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib -lboost_filesystem -lboost_system

INCLUDES=-I$(THRIFTROOT)/include -I.  -Igen-cpp -I../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

test1: test1.o NiceJSON.o $(GENCPPOBJ) gen-cpp-typelib/test_typelib.o
	g++  $(CPPFLAGS) -o test1 $^ -lthriftc $(LINKFLAGS)

jsontest: jsontest.o NiceJSON.o
	g++  $(CPPFLAGS) -o jsontest $^ -lthriftc $(LINKFLAGS)

thrift-gen-wirejson: wirejson_plugin.o $(PLUGINOBJ)
	g++ $(CPPFLAGS) -o thrift-gen-wirejson $^ -lthriftc $(LINKFLAGS)

thrift-gen-cpp-typelib: cpp_typelib_plugin.o $(PLUGINOBJ)
	g++ $(CPPFLAGS) -o thrift-gen-cpp-typelib $^ -lthriftc $(LINKFLAGS)

%.wirejson: %.thrift thrift-gen-wirejson
	PATH=.:${PATH} thrift -r -gen wirejson $< > $@

gen-cpp/%.o: gen-cpp/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

gen-cpp-typelib/%.o: gen-cpp-typelib/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o ThriftThrift test1 thrift-gen-wirejson thrift-gen-cpp-typelib *.wirejson jsontest

realclean:: clean
	rm -rf gen-*

NiceJSON.o: NiceJSON.h

foo: foo.o
	g++  $(CPPFLAGS) -o foo $^ -lthriftc $(LINKFLAGS)
