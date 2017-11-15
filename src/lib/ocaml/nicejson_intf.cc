#include "nicejson_intf.h"

namespace ocaml {
namespace nicejson {

using apache::thrift::nicejson::NiceJSON ;
using apache::thrift::nicejson::NiceJSONError ;

std::string
foo(int n) {
  std::string rv = std::to_string(n) ;
  return rv ;
}

std::tuple< std::string, int >
bar(std::string s, int n) {
  return std::tuple< std::string, int>{ s, n } ;
}

void prepend_typelib_directory(const std::string& name) {
  NiceJSON::prepend_typelib_directory(name) ;
}

void append_typelib_directory(const std::string& name) {
  NiceJSON::append_typelib_directory(name) ;
}

std::string install_typelib(const std::string& key, const string& serialized) {
  try {
    NiceJSON::install_typelib(key, serialized) ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
  return "" ;
}

std::string json_from_binary(const std::string& key, const std::string& type, const std::string& serialized) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  json actual = nj->marshal_from_binary(type, (uint8_t*)serialized.data(), serialized.size(), false) ;
  return actual.dump() ;
}

std::string binary_from_json(const std::string& key, const std::string& type, const std::string& json_serialized) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  json j = json::parse(json_serialized) ;
  std::string binary = nj->demarshal_to_binary(type, j) ;
  return binary ;
}

std::string require_typelib(const std::string& key) {
  try {
  NiceJSON::require_typelib(key) ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
  return "" ;
}

} // namespace nicejson
} // namespace ocaml
