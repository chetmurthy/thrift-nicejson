#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <iostream>

#define BOOST_TEST_MODULE NiceJSONTest2
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
#include "gen-cpp-typelib/test_typelib.h"

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

bool thrift_test::Bar::operator<(thrift_test::Bar const& that) const {
  if (this->a != that.a) return this->a < that.a ;
  if (this->b != that.b) return this->b < that.b ;
  return false ;
}

const string kTypelib = "thrift_test.test" ;
const string kType = "Bar" ;

BOOST_AUTO_TEST_CASE( Bar0 )
{
  thrift_test::Bar bar ;
  bar.__set_a(1) ;
  bar.__set_b("ugh") ;

  std::string serialized  = apache::thrift::ThriftBinaryString(bar) ;
  NiceJSON::prepend_typelib_directory("./gen-typelib") ;
  NiceJSON const * const nj = NiceJSON::require_typelib(kTypelib) ;
  json actual = nj->marshal_from_binary(kType, (uint8_t*)serialized.data(), serialized.size(), false) ;
  json expected = {{ "a", 1}, {"b", "ugh"}} ;
  BOOST_CHECK( actual == expected ) ;
}
