#define BOOST_TEST_MODULE NSTest
#include <boost/test/included/unit_test.hpp>
#include <boost/smart_ptr.hpp>

#include "ns_utils.h"

BOOST_AUTO_TEST_CASE( XX )
{
  BOOST_CHECK( ns_open("apache") == "namespace apache {" ) ;
  BOOST_CHECK( ns_open("apache.thrift.plugin") == "namespace apache { namespace thrift { namespace plugin {" ) ;

  BOOST_CHECK( ns_close("apache") == " }" ) ;
  BOOST_CHECK( ns_close("apache.thrift.plugin") == " } } }" ) ;

  BOOST_CHECK( ns_prefix("apache") == "apache" ) ;
  BOOST_CHECK( ns_prefix("apache.thrift.plugin") == "apache::thrift::plugin" ) ;

}

