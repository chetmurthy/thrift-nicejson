
all: thrift-gen-cpp-typelib

LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib -lboost_filesystem -lboost_system

INCLUDES=-I$(THRIFTROOT)/include -I. -I../lib -I../lib/gen-cpp -I../../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

thrift-gen-cpp-typelib: cpp_typelib_plugin.o ../lib/libnicejson.a
	g++ $(CPPFLAGS) -o thrift-gen-cpp-typelib $^ -lthriftc $(LINKFLAGS)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o thrift-gen-cpp-typelib

realclean:: clean
