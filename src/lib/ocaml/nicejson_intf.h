#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "NiceJSON.h"

#ifndef nicejson_h_included
#define nicejson_h_included

namespace ocaml {
namespace nicejson {

std::string
  foo(int n) ;

std::tuple< std::string, int >
  bar(std::string s, int n);

void prepend_typelib_directory(const std::string& name) ;

void append_typelib_directory(const std::string& name);

std::string install_typelib(const std::string& key, const string& serialized) ;

std::string json_from_binary(const std::string& key, const std::string& type, const std::string& serialized, std::string *out);

std::string binary_from_json(const std::string& key, const std::string& type, const std::string& json_serialized, std::string *out) ;

std::string require_typelib(const std::string& key) ;

} // namespace nicejson
} // namespace ocaml

#endif
