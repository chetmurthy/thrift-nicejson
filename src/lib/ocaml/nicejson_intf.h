#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#ifndef nicejson_h_included
#define nicejson_h_included

namespace nicejson {

std::string
  foo(int n) ;

std::tuple< std::string, int >
  bar(std::string s, int n);

} // namespace nicejson

#endif
