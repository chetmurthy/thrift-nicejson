
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift
PLUGIN_THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift

TESTSRC=$(wildcard gen-cpp/test*.cpp)
TYPELIBSRC=$(wildcard gen-cpp-typelib/*typelib.cpp)

TESTOBJ=$(patsubst %.cpp,%.o,$(TESTSRC))
TYPELIBOBJ=$(patsubst %.cpp,%.o,$(TYPELIBSRC))
GENCPPOBJ=$(TESTOBJ) $(TYPELIBOBJ)

all: test

test: test1
	./test1

gen-files:: gen-cpp gen-json gen-cpp-typelib

gen-cpp::
	$(THRIFT) --gen cpp -r test.thrift
	$(THRIFT) --gen cpp -r test0.thrift

gen-cpp-typelib::
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib test0.thrift
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib test.thrift
	PATH=../plugins:${PATH} thrift -r -gen cpp-typelib $(PLUGIN_THRIFT)

gen-json::
	$(THRIFT) --gen json -r $(PLUGIN_THRIFT)
	$(THRIFT) --gen json -r test.thrift
	$(THRIFT) --gen json -r test0.thrift

LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib -lboost_filesystem -lboost_system

INCLUDES=-I$(THRIFTROOT)/include -I. -I../lib -Igen-cpp -I../../../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

test1: test1.o $(GENCPPOBJ)
	g++  $(CPPFLAGS) -o test1 test1.o $(GENCPPOBJ) ../lib/libnicejson.a -lthriftc $(LINKFLAGS)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o test1

realclean:: clean
	rm -rf gen-*
