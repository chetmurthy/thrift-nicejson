#define BOOST_TEST_MODULE JSONTest
#include <boost/test/included/unit_test.hpp>
#include <boost/smart_ptr.hpp>

#include "TNiceJSONProtocol.h"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;

BOOST_AUTO_TEST_CASE( JSON1 )
{
  json j = parse_via_transport("{}") ;
  BOOST_CHECK( j.empty() );
  BOOST_CHECK( !j.is_null() );
  BOOST_CHECK( j.is_object() );
}

BOOST_AUTO_TEST_CASE( JSON2 )
{
  BOOST_CHECK_THROW( parse_via_transport("{") , nlohmann::detail::parse_error ) ;
}

BOOST_AUTO_TEST_CASE( JSON5 )
{
  std::string s = R"foo(
{ "a": 10,
  "b": "bar"
}
)foo" ;

  json j = parse_via_transport(s) ;

  json j2 = { { "a", 10 }, { "b", "bar" } } ;
  BOOST_CHECK( j == j2) ;
}
