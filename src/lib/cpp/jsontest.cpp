#include <list>
#include <boost/smart_ptr.hpp>

#include "gtest/gtest.h"

#include "json.hpp"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;

using nlohmann::json;

TEST( JSON, ParseEmpty )
{
  json j = "{}"_json ;
  ASSERT_TRUE( j.empty() );
  ASSERT_FALSE( j.is_null() );
  ASSERT_TRUE( j.is_object() );
}

TEST( JSON, Null )
{
  json j;
  ASSERT_TRUE( j.empty() );
  ASSERT_TRUE( j.is_null() );
  ASSERT_FALSE( j.is_object() );
}

TEST( JSON, Array )
{
  json j = std::vector<int>{} ;
  ASSERT_TRUE( j.empty() );
  ASSERT_FALSE( j.is_null() );
  ASSERT_TRUE( j.is_array() );
  ASSERT_TRUE( j.size() == 0 );
}

TEST( JSON, ParseArray )
{
  json j = "[]"_json ;
  ASSERT_TRUE( j.empty() );
  ASSERT_FALSE( j.is_null() );
  ASSERT_TRUE( j.is_array() );
  ASSERT_TRUE( j.size() == 0 );
}

TEST( JSON, Object )
{
  json j = R"foo(
{ "a": 10,
  "b": "bar"
}
)foo"_json ;

  json j2 = { { "a", 10 }, { "b", "bar" } } ;
  ASSERT_TRUE( j == j2) ;
}

TEST( JSON, Object2 )
{
  json j = R"foo(
{ "1": 10,
  "2": "bar"
}
)foo"_json ;

  ASSERT_TRUE( j.is_object()) ;
}

TEST( JSON, Object2StringException )
{
  json j = "{}"_json ;
  ASSERT_THROW ( j.get<string>() , std::exception ) ;
}

TEST( JSON, NumericLimits )
{
  json j ;
  j = std::numeric_limits<int32_t>::max() ;
  ASSERT_EQ(j.get<int32_t>(), std::numeric_limits<int32_t>::max()) ;

  j = 1ll + (int64_t)std::numeric_limits<int32_t>::max() ;
  ASSERT_EQ(j.get<int64_t>(), 1ll + (int64_t)std::numeric_limits<int32_t>::max()) ;
  ASSERT_NE(j.get<int32_t>(), 1ll + (int64_t)std::numeric_limits<int32_t>::max()) ;
}
