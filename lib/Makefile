
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift
PLUGIN_THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift

PLUGINSRC=$(wildcard gen-cpp/plugin*.cpp)

PLUGINOBJ=$(patsubst %.cpp,%.o,$(PLUGINSRC))

LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib -lboost_filesystem -lboost_system

INCLUDES=-I$(THRIFTROOT)/include -I.  -Igen-cpp -I../../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

all: libnicejson.a

libnicejson.a: NiceJSON.o ns_utils.o $(PLUGINOBJ)
	ar rv $@ $^
	ranlib $@

test: jsontest ns_utils_test
	./jsontest
	./ns_utils_test

gen-files:: gen-cpp

gen-cpp: $(PLUGIN_THRIFT)
	$(THRIFT) --gen cpp -r $(PLUGIN_THRIFT)

jsontest: jsontest.o NiceJSON.o
	g++  $(CPPFLAGS) -o jsontest $^ $(LINKFLAGS)

ns_utils_test: ns_utils_test.o ns_utils.o
	g++  $(CPPFLAGS) -o ns_utils_test $^ $(LINKFLAGS)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o  jsontest ns_utils_test libnicejson.a

realclean:: clean
	rm -rf gen-*

NiceJSON.o: NiceJSON.h
