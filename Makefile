
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift

all: gen-files thrift-gen-wirejson

test: test1

gen-files:: *.thrift gen-cpp gen-json

gen-cpp: test.thrift
	$(THRIFT) --gen cpp -r $(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift
	$(THRIFT) --gen cpp -r test.thrift

gen-json: test.thrift
	$(THRIFT) --gen json -r test.thrift

GENCPPSRC=$(wildcard gen-cpp/*.cpp)
GENCPPOBJ=$(patsubst %.cpp,%.o,$(GENCPPSRC))

LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib

INCLUDES=-I$(THRIFTROOT)/include -I.  -Igen-cpp -I../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

ThriftThrift: Thrift.o $(GENCPPOBJ)
	g++  $(CPPFLAGS) -o ThriftThrift $^ $(LINKFLAGS)

test1: test1.o NiceJSON.o $(GENCPPOBJ)
	g++  $(CPPFLAGS) -o test1 $^ -lthriftc $(LINKFLAGS)

thrift-gen-wirejson: wirejson_plugin.o $(GENCPPOBJ)
	g++ $(CPPFLAGS) -o thrift-gen-wirejson $^ -lthriftc $(LINKFLAGS)

test.wirejson::
	PATH=.:${PATH} thrift -r -gen wirejson test.thrift > test.wirejson

gen-cpp/%.o: gen-cpp/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o ThriftThrift test1 thrift-gen-wirejson *.wirejson

realclean:: clean
	rm -rf gen-*

NiceJSON.o: NiceJSON.h
