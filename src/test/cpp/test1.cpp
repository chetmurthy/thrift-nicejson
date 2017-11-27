#include <fstream>
#include <iostream>
#include <iostream>
#include <streambuf>
#include <string>
#include <tuple>

#include "gtest/gtest.h"
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
#include "gen-cpp-typelib/test_typelib.h"
#include "gen-cpp/tutorial_types.h"
#include "gen-cpp-typelib/tutorial_typelib.h"
#include "gen-cpp-typelib/shared_typelib.h"

#include "TNiceJSONProtocol.h"
#include "NiceJSON.h"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::map;
using std::list;
using std::set;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::nicejson;

const std::string kTestTypelib = "thrift_test.test" ;

bool thrift_test::Bar::operator<(thrift_test::Bar const& that) const {
  if (this->a != that.a) return this->a < that.a ;
  if (this->b != that.b) return this->b < that.b ;
  return false ;
}

template<typename T>
void RoundTrip(const string& structname, const json& json1, const std::string& typelib = kTestTypelib) {
  const NiceJSON& tt = *(NiceJSON::lookup_typelib(typelib)) ;

  T obj ;
  tt.demarshal(structname, json1, &obj) ;
  json json2 = tt.marshal(structname, obj) ;
  ASSERT_EQ( json1, json2)
    << "JSON not equal: should be " << json1.dump() << "\nbut instead " << json2.dump() ;
}

template<typename T>
void RoundTrip2(const string& structname, const T& arg, const std::string& typelib = kTestTypelib) {
  const NiceJSON& tt = *(NiceJSON::lookup_typelib(typelib)) ;

  json j = tt.marshal(structname, arg) ;
  T rv ;
  tt.demarshal(structname, j, &rv) ;
  ASSERT_EQ( rv, arg );
}

template<typename T>
void Mismatch(const string& structname, const json& startjson,const NiceJSON& oldtt, const json &expected,const bool permissive) {

  T obj ;
  thrift_test::demarshal(startjson, &obj) ;

  boost::shared_ptr<TMemoryBuffer> mem = apache::thrift::marshalToMemoryBuffer(obj) ;
  json actual = oldtt.marshal_from_binary(structname, mem, permissive) ;
  ASSERT_EQ( actual, expected)
    << "JSON not equal: should be " << expected.dump() << "\nbut instead " << actual.dump() ;
}

TEST( Bar, Ser )
{
  {
    thrift_test::Bar bar ;
    bar.__set_a(1) ;
    bar.__set_b("ugh") ;

    std::string serialized  = apache::thrift::ThriftBinaryString(bar) ;
  }
}

TEST( Bar, RoundTrip )
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
    
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()), TMemoryBuffer::COPY);
      bar2.read(p.get());

    }

    ASSERT_EQ(bar, bar2) ;
  }
}

TEST( Ha, RoundTrip )
{
  thrift_test::Ha ha ;
  ha.__set_e(thrift_test::E::C) ;

  RoundTrip2<thrift_test::Ha>("Ha", ha) ;
}

TEST( Typelib, Lookup )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin.plugin")) ;
    {
      std::string serialized = apache::thrift::ThriftDebugString(tt.it()) ;
      cout << serialized << std::endl ;
    }
}

TEST( Bar, DebugSer )
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

TEST( Bar, RoundTrips )
{
  
  ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 } }), std::exception );
 ;

 ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", 1 + (int)std::numeric_limits<int8_t>::max() } }), std::exception ) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::min() } }) ;

 ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", 1 + (int)std::numeric_limits<int16_t>::max() } }), std::exception ) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::min() } }) ;

 ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", 1ll + (int64_t)std::numeric_limits<int32_t>::max() } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::max() } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::min() } }) ;

 ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", 1 } }), std::exception ) ;

 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::max()) } }) ;
 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::min()) } }) ;

 ASSERT_THROW( RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(2.0 * (double)std::numeric_limits<int64_t>::max()) } }),
		    std::out_of_range );

 RoundTrip<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f9", 3.1415 } }) ;
}

TEST( Bar, Mismatch )
{
  const NiceJSON& oldtt = *(NiceJSON::lookup_typelib("thrift_test0.test0")) ;

  Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" },  }, oldtt, "{}"_json, false) ;
  Mismatch<thrift_test::Bar>(
			     "Bar", { { "a", 1 }, { "b", "ugh" },  },
			     oldtt,
    { { "4", 1 }, { "5", "ugh" } },
			     true) ;

  Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f6", std::numeric_limits<int8_t>::min() } },
			      oldtt,
    { { "4", 1 } , { "5", "ugh" } , { "6", std::numeric_limits<int8_t>::min() } },
			     true) ;

  Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::max() } },
			     oldtt,
			     { { "4", 1 } , { "5", "ugh" } , { "7", std::numeric_limits<int16_t>::max() } },
			     true) ;
  Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f7", std::numeric_limits<int16_t>::min() } },
			     oldtt,
			     { { "4", 1 } , { "5", "ugh" } , { "7", std::numeric_limits<int16_t>::min() } },
			     true) ;

 Mismatch<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::max() } },
			     oldtt,
			     { { "5", "ugh" } , { "4", std::numeric_limits<int32_t>::max() } },
			     true) ;
 Mismatch<thrift_test::Bar>("Bar", { { "b", "ugh" }, { "a", std::numeric_limits<int32_t>::min() } },
			     oldtt,
			     { { "5", "ugh" } , { "4", std::numeric_limits<int32_t>::min() } },
			     true) ;

 Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::max()) } },
			     oldtt,
   { { "4", 1 } , { "5", "ugh" } , { "8", std::to_string(std::numeric_limits<int64_t>::max()) } },
			     true) ;
 Mismatch<thrift_test::Bar>("Bar", { { "a", 1 }, { "b", "ugh" }, { "f8", std::to_string(std::numeric_limits<int64_t>::min()) } },
			     oldtt,
   { { "4", 1 } , { "5", "ugh" } , { "8", std::to_string(std::numeric_limits<int64_t>::min()) } },
			     true) ;

}

TEST( Boo, RoundTrip )
{
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[]"_json } }) ;
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[]]]"_json } }) ;
  RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[1, 2]]]"_json } }) ;

  ASSERT_THROW ( RoundTrip<thrift_test::Boo>("Boo", { { "l", "[[[[]]]]"_json } }),
		      std::exception ) ;
  ASSERT_THROW ( RoundTrip<thrift_test::Boo>("Boo", { { "l", "[1]"_json } }),
		      std::exception ) ;
}

TEST( Goo, RoundTrip )
{
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[]"_json } }) ;
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[]]]"_json } }) ;
  RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[1, 2]]]"_json } }) ;

  ASSERT_THROW ( RoundTrip<thrift_test::Goo>("Goo", { { "l", "[[[[]]]]"_json } }),
		      std::exception ) ;
  ASSERT_THROW ( RoundTrip<thrift_test::Goo>("Goo", { { "l", "[1]"_json } }),
		      std::exception ) ;
}

TEST( Foo, RoundTrip )
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

TEST( Boo, RoundTrip2 )
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

TEST( Boo, RoundTrip3 )
{
  json j = R"foo(
{ "l": [[[1, 2, 3]]],
  "m": [ [ { "a": 1, "b": "ugh" }, { "a": 1, "b": "ugh" } ] ],
  "n": [ [ 1, "foo" ], [ 2, "bar" ] ]
}
)foo"_json ;

  RoundTrip<thrift_test::Boo>("Boo", j) ;
}

TEST( Plugin, RoundTrip )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin.plugin")) ;

  std::cout << apache::thrift::ThriftDebugString(tt.it()) << std::endl ;

  json j = tt.marshal("GeneratorInput", tt.it()) ;
  apache::thrift::plugin::GeneratorInput x ;
  tt.demarshal("GeneratorInput", j, &x) ;
  ASSERT_EQ( tt.it(), x ) ;
}

TEST( Plugin, ToJSON )
{
  const NiceJSON& testtt = *(NiceJSON::lookup_typelib(kTestTypelib)) ;
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("apache.thrift.plugin.plugin")) ;

  json j = tt.marshal("GeneratorInput", testtt.it()) ;
  std::cout << j << std::endl ;
}

TEST( S2_foo_args, RoundTrip )
{
  thrift_test::S2_foo_args args ;
  args.n = 1 ;
  args.__isset.n = true ;
  thrift_test::Bar v;
  v.a = 10 ;
  v.b = "foo" ;
  args.w = v ;
  args.__isset.w = true ;

  RoundTrip2<thrift_test::S2_foo_args>("S2_foo_args", args) ;
}

std::string memory_buffer_contents(boost::shared_ptr<TMemoryBuffer>& mem) {
  uint8_t* buf;
  uint32_t size;
  mem->getBuffer(&buf, &size);
  std::string contents((char*)buf, (unsigned int)size);
  return contents ;
}

TMemoryBuffer* new_memory_buffer_with_contents(const std::string& contents) {
  TMemoryBuffer* mem = new TMemoryBuffer() ;
  mem->resetBuffer((uint8_t*)(contents.data()), contents.length(), TMemoryBuffer::COPY) ;
  return mem ;
}

template<typename T>
void MessageRoundTrip2(const string& service, const string& operation,
		      const TMessageType messageType,
		      const int32_t seqid,
		      const T& arg) {

  using apache::thrift::protocol::TNiceJSONProtocol ;

  boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer()) ;
  boost::shared_ptr<TNiceJSONProtocol> proto(new TNiceJSONProtocol(kTestTypelib, service, trans)) ;
  proto->writeMessageBegin(operation, messageType, seqid) ;
  arg.write(proto.get()) ;
  proto->writeMessageEnd() ;

  string read_operation ;
  TMessageType read_messageType ;
  int32_t read_seqid ;
  proto->readMessageBegin(read_operation, read_messageType, read_seqid) ;
  T rv ;
  rv.read(proto.get()) ;
  proto->readMessageEnd() ;
  ASSERT_EQ( read_operation, operation ) ;
  ASSERT_EQ( read_messageType, messageType ) ;
  ASSERT_EQ( read_seqid, seqid ) ;
  ASSERT_EQ( rv, arg);
}

TEST( S2_foo_args, RoundTrip2 )
{
  thrift_test::S2_foo_args args ;
  args.n = 1 ;
  args.__isset.n = true ;
  thrift_test::Bar v;
  v.a = 10 ;
  v.b = "foo" ;
  args.w = v ;
  args.__isset.w = true ;

  using apache::thrift::protocol::TNiceJSONProtocol ;

  boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer()) ;
  boost::shared_ptr<TNiceJSONProtocol> proto(new TNiceJSONProtocol(kTestTypelib, "S2", trans)) ;
  proto->writeMessageBegin("foo", T_CALL, 1l) ;
  args.write(proto.get()) ;
  proto->writeMessageEnd() ;

  std::cout << memory_buffer_contents(trans) ;
}

TEST( S2_foo_args, RoundTrip3 )
{
  thrift_test::S2_foo_args args ;
  args.n = 1 ;
  args.__isset.n = true ;
  thrift_test::Bar v;
  v.a = 10 ;
  v.b = "foo" ;
  args.w = v ;
  args.__isset.w = true ;

  MessageRoundTrip2<thrift_test::S2_foo_args>("S2", "foo", T_CALL, 1l, args) ;
}

TEST( S2_foo_args, RoundTrip4 )
{
  thrift_test::S2_foo_args args ;
  const std::string msg = R"FOO(
{"body":{"n":1,"w":{"a":10,"b":"foo"}},"name":"foo","seqid":1,"type":"call"}
)FOO" ;

  boost::shared_ptr<TMemoryBuffer> trans(new_memory_buffer_with_contents(msg)) ;
  boost::shared_ptr<TNiceJSONProtocol> proto(new TNiceJSONProtocol(kTestTypelib, "S2", trans)) ;
  string read_operation ;
  TMessageType read_messageType ;
  int32_t read_seqid ;
  proto->readMessageBegin(read_operation, read_messageType, read_seqid) ;
  args.read(proto.get()) ;
  proto->readMessageEnd() ;
  ASSERT_EQ( read_operation, "foo" ) ;
  ASSERT_EQ( read_messageType, T_CALL ) ;
  ASSERT_EQ( read_seqid, 1l ) ;
}

TEST( S2_foo_args, RoundTrip5 )
{
  thrift_test::S2_foo_args args ;
  const std::string msg = R"FOO(
{"body":{"n":1,"w":{"a":10,"b":
)FOO" ;

  boost::shared_ptr<TMemoryBuffer> trans(new_memory_buffer_with_contents(msg)) ;
  boost::shared_ptr<TNiceJSONProtocol> proto(new TNiceJSONProtocol(kTestTypelib, "S2", trans)) ;
  string read_operation ;
  TMessageType read_messageType ;
  int32_t read_seqid ;
  ASSERT_THROW( proto->readMessageBegin(read_operation, read_messageType, read_seqid) ,
		     apache::thrift::transport::TTransportException );
}

TEST( S2_foo_args, RoundTrip6 )
{
  thrift_test::S2_foo_args args ;
  const std::string msg = R"FOO(
{"body":{"n":1,"w":{"a":10,"b":"fooasdfasdfasfasdsaasdfasd":1,"type":"call"}
)FOO" ;

  boost::shared_ptr<TMemoryBuffer> trans(new_memory_buffer_with_contents(msg)) ;
  boost::shared_ptr<TNiceJSONProtocol> proto(new TNiceJSONProtocol(kTestTypelib, "S2", trans)) ;
  string read_operation ;
  TMessageType read_messageType ;
  int32_t read_seqid ;
  ASSERT_THROW( proto->readMessageBegin(read_operation, read_messageType, read_seqid) ,
		     apache::thrift::protocol::TProtocolException );
}

TEST( S2_foo_result, RoundTrip )
{
  thrift_test::S2_foo_result result ;
  result.success = 42 ;
  result.__isset.success = true ;

  RoundTrip2<thrift_test::S2_foo_result>("S2_foo_result", result) ;
}

TEST( S2_foo_result, RoundTrip2 )
{
  thrift_test::S2_foo_result result ;
  thrift_test::InvalidOperation2 ouch2;
  ouch2.__set_whatOp(36) ;
  ouch2.__set_why("foo") ;
  result.ouch2 = ouch2 ;
  result.__isset.ouch2 = true ;

  RoundTrip2<thrift_test::S2_foo_result>("S2_foo_result", result) ;
}

TEST( shared_SharedStruct, RoundTrip )
{
  shared::SharedStruct x ;
  x.__set_key(42) ;
  x.__set_value("foo") ;

  RoundTrip2<shared::SharedStruct>("shared::SharedStruct", x, "tutorial.tutorial") ;
  RoundTrip2<shared::SharedStruct>("SharedStruct", x, "shared.shared") ;
}

TEST( service_struct_names, generate )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("tutorial.tutorial")) ;

  ASSERT_EQ( tt.service_struct_names("Calculator", "add"),
	     (pair<string, string>{ "Calculator_add_args", "Calculator_add_result" }) ) ;

  ASSERT_EQ( tt.service_struct_names("Calculator", "getStruct"),
	      (pair<string, string>{ "shared::SharedService_getStruct_args", "shared::SharedService_getStruct_result" }) ) ;
}

TEST( service_struct_names, generate_2 )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib("shared.shared")) ;

  ASSERT_EQ( tt.service_struct_names("SharedService", "getStruct"),
	     (pair<string, string>{ "SharedService_getStruct_args", "SharedService_getStruct_result" }) ) ;
}

TEST( TApplicationException_0, JSON )
{
  const NiceJSON& tt = *(NiceJSON::lookup_typelib(kTestTypelib)) ;
  ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN, "eh?") ;

  json j = tt.marshal("TApplicationException", x) ;
  std::cout << j.dump() << std::endl ;

  ::apache::thrift::TApplicationException x2 ;
  tt.demarshal("TApplicationException", j, &x2) ;
  ASSERT_EQ( x.getType(), x2.getType() ) ;
  ASSERT_EQ( std::string(x.what()), std::string(x2.what()) ) ;
  json j2 = tt.marshal("TApplicationException", x2) ;
  ASSERT_EQ( j, j2 ) ;

}
