#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "nicejson_intf.h"

namespace nicejson {

std::string
foo(int n) {
  std::string rv = std::to_string(n) ;
  std::cout << "rv = " << rv << std::endl ;
  return rv ;
}

std::tuple< std::string, int >
bar(std::string s, int n) {
  return std::tuple< std::string, int>{ s, n } ;
}

} // namespace nicejson
