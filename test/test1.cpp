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
using namespace apache::thrift::nicejson;

namespace apache {
namespace thrift {

template <typename ThriftStruct>
boost::shared_ptr<TMemoryBuffer> marshalToMemoryBuffer(const ThriftStruct& ts) {
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer());
  TBinaryProtocol protocol(trans);

  ts.write(&protocol);
  return trans ;
}
}
}

namespace thrift_test {
extern struct StaticInitializer_test {
  StaticInitializer_test();
  apache::thrift::nicejson::NiceJSON json_ ;
} json_ ;

}

template<typename T>
void RoundTrip(const string& structname, const json& json1) {
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("thrift_test/test")) ;

  T obj ;
  tt.demarshal(structname, json1, &obj) ;
  json json2 = tt.marshal(structname, obj) ;
  BOOST_CHECK_MESSAGE( json1 == json2,
		       "JSON not equal: should be " << json1.dump() << "\nbut instead " << json2.dump());
}

template<typename T>
void RoundTrip2(const string& structname, const T& arg) {
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("thrift_test/test")) ;

  json j = tt.marshal(structname, arg) ;
  T rv ;
  tt.demarshal(structname, j, &rv) ;
  BOOST_CHECK( rv == arg );
}

template<typename T>
void Mismatch(const string& structname, const json& startjson, const NiceJSON& newtt,const NiceJSON& oldtt, const json &expected,const bool permissive) {

  T obj ;
  newtt.demarshal(structname, startjson, &obj) ;

  boost::shared_ptr<TMemoryBuffer> mem = apache::thrift::marshalToMemoryBuffer(obj) ;
  json actual = oldtt.marshal_from_binary(structname, mem, permissive) ;
  BOOST_CHECK_MESSAGE( actual == expected,
		       "JSON not equal: should be " << expected.dump() << "\nbut instead " << actual.dump());
}

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

BOOST_AUTO_TEST_CASE( Ha1 )
{
  thrift_test::Ha ha ;
  ha.__set_e(thrift_test::E::C) ;

  RoundTrip2<thrift_test::Ha>("Ha", ha) ;
}

BOOST_AUTO_TEST_CASE( IO1 )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin/plugin")) ;
    {
      std::string serialized = apache::thrift::ThriftDebugString(tt.it()) ;
      cout << serialized << std::endl ;
    }
}

BOOST_AUTO_TEST_CASE( Bar1 )
{
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

BOOST_AUTO_TEST_CASE( Bar2 )
{
  
  BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 } }), std::exception );
 ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", 1 + (int)std::numeric_limits<int8_t>::max() } }), std::exception ) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", 1 + (int)std::numeric_limits<int16_t>::max() } }), std::exception ) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", 1ll + (int64_t)std::numeric_limits<int32_t>::max() } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", 1 } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::max()) } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::min()) } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(2.0 * (double)std::numeric_limits<int64_t>::max()) } }),
		    std::out_of_range );
}

BOOST_AUTO_TEST_CASE( BarMismatch )
{
  const NiceJSON& newtt = *(NiceJSON::lookup_typelib("thrift_test/test")) ;
  const NiceJSON& oldtt = *(NiceJSON::lookup_typelib("thrift_test0/test0")) ;

  Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" },  }, newtt, oldtt, "{}"_json, false) ;
  Mismatch<thrift_test::Bar>(
			     "Bar", { { "a", 1 }, { "b", "ugh" },  },
			     newtt, oldtt,
    { { "4", 1 }, { "5", "ugh" } },
			     true) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", 1 + (int)std::numeric_limits<int16_t>::max() } }), std::exception ) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", 1ll + (int64_t)std::numeric_limits<int32_t>::max() } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::min() } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", 1 } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::max()) } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::min()) } }) ;

 BOOST_CHECK_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(2.0 * (double)std::numeric_limits<int64_t>::max()) } }),
		    std::out_of_range );
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

BOOST_AUTO_TEST_CASE( Boo2 )
{
  thrift_test::Boo boo ;
  boo.l = {} ;
  thrift_test::Bar v;
  v.a = 10 ;
  v.b = "foo" ;
  boo.m[v] = v ;
  boo.__isset.m = true ;

  RoundTrip2<thrift_test::Boo>("Boo", boo) ;
}

BOOST_AUTO_TEST_CASE( Boo3 )
{
  json j = R"foo(
{ "l": [[[1, 2, 3]]],
  "m": [ [ { "a": 1, "b": "ugh" }, { "a": 1, "b": "ugh" } ] ],
  "n": [ [ 1, "foo" ], [ 2, "bar" ] ]
}
)foo"_json ;

  RoundTrip<thrift_test::Boo>("Boo", j) ;
}

BOOST_AUTO_TEST_CASE( Plugin1 )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin/plugin")) ;

  std::cout << apache::thrift::ThriftDebugString(tt.it()) << std::endl ;

  json j = tt.marshal("GeneratorInput", tt.it()) ;
  apache::thrift::plugin::GeneratorInput x ;
  tt.demarshal("GeneratorInput", j, &x) ;
  BOOST_CHECK( tt.it() == x ) ;
}

BOOST_AUTO_TEST_CASE( TestIDLAsJSON )
{
  const NiceJSON& testtt = *(NiceJSON::lookup_typelib("thrift_test/test")) ;
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin/plugin")) ;

  json j = tt.marshal("GeneratorInput", testtt.it()) ;
  std::cout << j << std::endl ;
}

