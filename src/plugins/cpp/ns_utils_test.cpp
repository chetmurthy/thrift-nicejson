#include "gtest/gtest.h"

#include <boost/smart_ptr.hpp>

#include "ns_utils.h"

TEST( NSUtils, Test )
{
  ASSERT_EQ( ns_open("apache"), "namespace apache {" ) ;
  ASSERT_EQ( ns_open("apache.thrift.plugin"), "namespace apache { namespace thrift { namespace plugin {" ) ;

  ASSERT_EQ( ns_close("apache"), " }" ) ;
  ASSERT_EQ( ns_close("apache.thrift.plugin"), " } } }" ) ;

  ASSERT_EQ( ns_prefix("apache"), "apache" ) ;
  ASSERT_EQ( ns_prefix("apache.thrift.plugin"), "apache::thrift::plugin" ) ;

}

