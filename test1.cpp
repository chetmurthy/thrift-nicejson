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

BOOST_AUTO_TEST_CASE( Bar )
{
  {
    thrift_test::Bar bar ;
    bar.__set_a(1) ;
    bar.__set_b("ugh") ;

    std::string serialized  = apache::thrift::ThriftJSONString(bar) ;

    thrift_test::Bar bar2 ;
    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()));
      bar2.read(p.get());

    }

    BOOST_CHECK(bar == bar2) ;
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

template<typename T>
void RoundTrip(const string& structname, const json& json1) {
  std::string ss = file_contents("test.wirejson") ;
  NiceJSON tt(ss) ;

    T obj ;
    tt.demarshal(structname, json1, &obj) ;
    json json2 = tt.marshal(structname, obj) ;
    BOOST_CHECK( json1 == json2 );
}

BOOST_AUTO_TEST_CASE( Bar2 )
{
  RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" } }) ;
  
  BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 } }), std::exception );
 ;
}

BOOST_AUTO_TEST_CASE( Boo )
{
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[]"_json } }) ;
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[]]]"_json } }) ;
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[1, 2]]]"_json } }) ;

  BOOST_CHECK_THROW ( RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[[]]]]"_json } }),
		      std::exception ) ;
  BOOST_CHECK_THROW ( RoundTrip<thrift_test::Boo>("Boo", { { "l", "[1]"_json } }),
		      std::exception ) ;
}

BOOST_AUTO_TEST_CASE( Goo )
{
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[]"_json } }) ;
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[]]]"_json } }) ;
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[1, 2]]]"_json } }) ;

  BOOST_CHECK_THROW ( RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[[]]]]"_json } }),
		      std::exception ) ;
  BOOST_CHECK_THROW ( RoundTrip<thrift_test::Goo>("Goo", { { "l", "[1]"_json } }),
		      std::exception ) ;
}

BOOST_AUTO_TEST_CASE( Foo )
{
  json j = R"foo(
{ "a": 10,
  "b": "bar",
  "c": { "a": 1, "b": "ugh" },
  "d": [{ "a": 1, "b": "ugh" }, { "a": 1, "b": "ugh" }],
  "k": { "a": 10, "b": 20 }
}
)foo"_json ;

  std::cout << j << std::endl ;
  RoundTrip<thrift_test::Foo>("Foo", j) ;

  //  RoundTrip<thrift_test::Foo>("Foo", j) ;
}

bool thrift_test::Bar::operator<(thrift_test::Bar const& that) const {
  if (this->a != that.a) return this->a < that.a ;
  if (this->b != that.b) return this->b < that.b ;
  return false ;
}
