#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#define BOOST_TEST_MODULE NiceJSONTest
#include <boost/test/included/unit_test.hpp>
#include <boost/smart_ptr.hpp>

#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TFileTransport.h>
#include "thrift/transport/TFDTransport.h"

#include "gen-cpp/plugin_types.h"
#include "gen-cpp/test_types.h"

#include "NiceJSON.h"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

BOOST_AUTO_TEST_CASE( Foo1 )
{
  using namespace thrift_test;
  {
    Foo a ;
    a.__set_a(1) ;
    a.__set_b("ugh") ;
    a.__set_c(Bar()) ;
    a.c.__set_a(2) ;
    a.c.__set_b("argh");
    if (true) {
      a.d.push_back(a.c) ;
      a.d.push_back(a.c) ;
      a.e["foo"] = a.c ;
      a.e["bar"] = a.c ;
      a.g.insert(32) ;
      a.j = map<string, set<int32_t> >{
	{"foo", {1,2,3}},
	{"bar", {4,5,6}},
      } ;
    }
    std::string serialized ;
    {
      serialized = apache::thrift::ThriftJSONString(a) ;
      cout << serialized << std::endl ;
    }

    Foo a2 ;
    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()));
      a2.read(p.get());

    }

    assert(a == a2) ;

    {
      std::string serialized = apache::thrift::ThriftDebugString(a) ;
      cout << serialized << std::endl ;
    }
  }
}

BOOST_AUTO_TEST_CASE( Bar )
{
  {
    thrift_test::Bar bar ;
    bar.__set_a(1) ;
    bar.__set_b("ugh") ;

    std::string serialized ;
    {
      serialized = apache::thrift::ThriftJSONString(bar) ;
      cout << serialized << std::endl ;
    }

    thrift_test::Bar bar2 ;
    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()));
      bar2.read(p.get());

    }

    assert(bar == bar2) ;

    {
      std::string serialized = apache::thrift::ThriftDebugString(bar) ;
      cout << serialized << std::endl ;
    }
  }
}

BOOST_AUTO_TEST_CASE( IO1 )
{
  std::string ss = file_contents("test.wirejson") ;
  NiceJSON tt(ss) ;
    {
      std::string serialized = apache::thrift::ThriftDebugString(tt.it()) ;
      cout << serialized << std::endl ;
    }
}

BOOST_AUTO_TEST_CASE( Bar1 )
{
  std::string ss = file_contents("test.wirejson") ;
  NiceJSON tt(ss) ;
    {
      std::string serialized = apache::thrift::ThriftDebugString(tt.it()) ;
      cout << serialized << std::endl ;
    }

    json bar_json = { { "a", 1 }, { "b", "ugh" } } ;
    std::cout << bar_json << std::endl ;

    boost::shared_ptr<TTransport> trans(new TMemoryBuffer());
    TBinaryProtocol protocol(trans);
    {
      protocol.writeStructBegin("Bar");

      protocol.writeFieldBegin("a", ::apache::thrift::protocol::T_I32, 4);
      protocol.writeI32(1);
      protocol.writeFieldEnd();

      protocol.writeFieldBegin("b", ::apache::thrift::protocol::T_STRING, 5);
      protocol.writeString(std::string("ugh"));
      protocol.writeFieldEnd();

      protocol.writeFieldStop();
      protocol.writeStructEnd();
    }
    thrift_test::Bar bar ;
    bar.read(&protocol) ;
    cout << apache::thrift::ThriftDebugString(bar) << std::endl ;
}
