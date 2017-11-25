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
    return "" ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
}

std::string json_from_binary(const std::string& key, const std::string& type, const std::string& serialized, std::string *out) {
  try {
    NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
    json actual = nj->marshal_from_binary(type, (uint8_t*)serialized.data(), serialized.size(), false) ;
    *out = actual.dump() ;
    return "" ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
}

std::string binary_from_json(const std::string& key, const std::string& type, const std::string& json_serialized, std::string *out) {
  try {
    NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
    json j = json::parse(json_serialized) ;
    *out = nj->demarshal_to_binary(type, j) ;
    return "" ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
}

std::string require_typelib(const std::string& key) {
  try {
    NiceJSON::require_typelib(key) ;
    return "" ;
  } catch (NiceJSONError e) {
    return e.what() ;
  } catch (apache::thrift::protocol::TProtocolException e) {
    return e.what() ;
  }
}

const std::string
  service_struct_name_args(const std::string key,
			   const std::string& service,
			   const std::string& operation) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  return nj->service_struct_names(service, operation).first ;
}

const std::string
  service_struct_name_result(const std::string key,
			   const std::string& service,
			   const std::string& operation) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  return nj->service_struct_names(service, operation).second ;
}


} // namespace nicejson
} // namespace ocaml
