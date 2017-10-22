#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#define BOOST_TEST_MODULE JSONTest
#include <boost/test/included/unit_test.hpp>
#include <boost/smart_ptr.hpp>

#include "json.hpp"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;

using nlohmann::json;

BOOST_AUTO_TEST_CASE( JSON1 )
{
  json j = "{}"_json ;
  BOOST_CHECK( j.empty() );
  BOOST_CHECK( !j.is_null() );
  BOOST_CHECK( j.is_object() );
}

BOOST_AUTO_TEST_CASE( JSON2 )
{
  json j;
  BOOST_CHECK( j.empty() );
  BOOST_CHECK( j.is_null() );
  BOOST_CHECK( !j.is_object() );
}

BOOST_AUTO_TEST_CASE( JSON3 )
{
  json j = std::vector<int>{} ;
  BOOST_CHECK( j.empty() );
  BOOST_CHECK( !j.is_null() );
  BOOST_CHECK( j.is_array() );
  BOOST_CHECK( j.size() == 0 );
}

BOOST_AUTO_TEST_CASE( JSON4 )
{
  json j = "[]"_json ;
  BOOST_CHECK( j.empty() );
  BOOST_CHECK( !j.is_null() );
  BOOST_CHECK( j.is_array() );
  BOOST_CHECK( j.size() == 0 );
}

BOOST_AUTO_TEST_CASE( JSON5 )
{
  json j = R"foo(
{ "a": 10,
  "b": "bar"
}
)foo"_json ;

  json j2 = { { "a", 10 }, { "b", "bar" } } ;
  BOOST_CHECK( j == j2) ;
}
