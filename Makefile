
#THRIFT=thrift
THRIFT=$(THRIFTROOT)/src/thrift/compiler/cpp/thrift

all: gen-files thrift-gen-wirejson

test: test1 jsontest test.wirejson
	./test1
	./jsontest

gen-files:: *.thrift gen-cpp gen-json

gen-cpp: test.thrift
	$(THRIFT) --gen cpp -r $(THRIFTROOT)/src/thrift/compiler/cpp/src/thrift/plugin/plugin.thrift
	$(THRIFT) --gen cpp -r test.thrift

gen-json: test.thrift
	$(THRIFT) --gen json -r test.thrift

TESTSRC=$(wildcard gen-cpp/test*.cpp)
PLUGINSRC=$(wildcard gen-cpp/plugin*.cpp)

TESTOBJ=$(patsubst %.cpp,%.o,$(TESTSRC))
PLUGINOBJ=$(patsubst %.cpp,%.o,$(PLUGINSRC))
GENCPPOBJ=$(TESTOBJ) $(PLUGINOBJ)


LINKFLAGS=-L$(THRIFTROOT)/lib -lthrift -lssl -lcrypto -Wl,-rpath -Wl,/home/chet/Hack/thrift-0.10.0/lib

INCLUDES=-I$(THRIFTROOT)/include -I.  -Igen-cpp -I../json/src
DEBUG=-g
CPPFLAGS=$(INCLUDES)  -Wall -Wextra -pedantic $(DEBUG) -std=c++11

test1: test1.o NiceJSON.o $(GENCPPOBJ)
	g++  $(CPPFLAGS) -o test1 $^ -lthriftc $(LINKFLAGS)

jsontest: jsontest.o NiceJSON.o
	g++  $(CPPFLAGS) -o jsontest $^ -lthriftc $(LINKFLAGS)

thrift-gen-wirejson: wirejson_plugin.o $(PLUGINOBJ)
	g++ $(CPPFLAGS) -o thrift-gen-wirejson $^ -lthriftc $(LINKFLAGS)

test.wirejson: thrift-gen-wirejson test.thrift
	PATH=.:${PATH} thrift -r -gen wirejson test.thrift > test.wirejson

gen-cpp/%.o: gen-cpp/%.cpp
	g++ $(CPPFLAGS) -c $< -o $@

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf */*.o *.o ThriftThrift test1 thrift-gen-wirejson *.wirejson jsontest

realclean:: clean
	rm -rf gen-*

NiceJSON.o: NiceJSON.h
