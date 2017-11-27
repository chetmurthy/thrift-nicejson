#include "gtest/gtest.h"
#include <boost/smart_ptr.hpp>

#include "TNiceJSONProtocol.h"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;

TEST( JSON, Null )
{
  json j = parse_via_transport("{}") ;
  ASSERT_TRUE( j.empty() );
  ASSERT_FALSE( j.is_null() );
  ASSERT_TRUE( j.is_object() );
}

TEST( JSON, BrokenText )
{
  ASSERT_THROW( parse_via_transport("{") , apache::thrift::transport::TTransportException ) ;
}

TEST( JSON, Object )
{
  std::string s = R"foo(
{ "a": 10,
  "b": "bar"
}
)foo" ;

  json j = parse_via_transport(s) ;

  json j2 = { { "a", 10 }, { "b", "bar" } } ;
  ASSERT_EQ( j, j2) ;
}
