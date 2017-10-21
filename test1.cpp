#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#define BOOST_TEST_MODULE MyTest
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

int add( int i, int j ) { return i+j; }

BOOST_AUTO_TEST_CASE( my_test )
{
    // seven ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error

    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error

    if( add( 2,2 ) != 4 )
      BOOST_ERROR( "Ouch..." );            // #3 continues on error

    if( add( 2,2 ) != 4 )
      BOOST_FAIL( "Ouch..." );             // #4 throws on error

    if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error

    BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                         "add(..) result: " << add( 2,2 ) );

    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
}

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
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TDebugProtocol> p(new TDebugProtocol(buf)); 
    
      a.write(p.get()) ;
      cout << buf->getBufferAsString() << std::endl ;
    }
  }
}

#include "json.hpp"

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
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TDebugProtocol> p(new TDebugProtocol(buf)); 

      bar.write(p.get()) ;
      cout << buf->getBufferAsString() << std::endl ;
    }
  }
}

BOOST_AUTO_TEST_CASE( BarIDL )
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
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()));

      NiceJSON<thrift_test::Bar> tt(buf) ;
    
      bar2 = tt.it() ;
    }

    assert(bar == bar2) ;

    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TDebugProtocol> p(new TDebugProtocol(buf)); 

      bar.write(p.get()) ;
      cout << buf->getBufferAsString() << std::endl ;
    }
  }
}

BOOST_AUTO_TEST_CASE( IO1 )
{
  std::string ss = file_contents("test.wirejson") ;
  NiceJSON<apache::thrift::plugin::GeneratorInput> tt(ss) ;
    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TDebugProtocol> p(new TDebugProtocol(buf)); 

      tt.it().write(p.get()) ;
      cout << buf->getBufferAsString() << std::endl ;
    }
}
