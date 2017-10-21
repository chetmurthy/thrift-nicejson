
#include "NiceJSON.h"

namespace apache {
namespace thrift {
namespace plugin {

struct ThriftPluginError : public apache::thrift::TException {
  ThriftPluginError(const std::string& msg) : apache::thrift::TException(msg) {}
};

enum t_const_value_type { CV_INTEGER, CV_DOUBLE, CV_STRING, CV_MAP, CV_LIST, CV_IDENTIFIER };

t_const_value_type const_value_case(const t_const_value& v) {
  if (v.__isset.map_val)
    return CV_MAP;
  if (v.__isset.list_val)
    return CV_LIST;
  if (v.__isset.string_val)
    return CV_STRING;
  if (v.__isset.integer_val)
    return CV_INTEGER;
  if (v.__isset.double_val)
    return CV_DOUBLE;
  if (v.__isset.identifier_val)
    return CV_IDENTIFIER;
  if (v.__isset.enum_val)
    return CV_IDENTIFIER;
  throw ThriftPluginError("Unknown const value type");
}

bool t_const_value::operator<(const t_const_value& that) const {
  t_const_value_type t1 = const_value_case(*this);
  t_const_value_type t2 = const_value_case(that);
  if (t1 != t2)
    return t1 < t2;
  switch (t1) {
  case CV_INTEGER:
    return integer_val < that.integer_val;
  case CV_DOUBLE:
    return double_val < that.double_val;
  case CV_STRING:
    return string_val < that.string_val;
  case CV_MAP:
    if (that.map_val.empty())
      return false;
    else if (map_val.empty())
      return true;
    else
      return map_val.begin()->first < that.map_val.begin()->first;
  case CV_LIST:
    if (that.list_val.empty())
      return false;
    else if (list_val.empty())
      return true;
    else
      return list_val.front() < that.list_val.front();
  case CV_IDENTIFIER:
    return integer_val < that.integer_val;
  }
  throw ThriftPluginError("Unknown const value type");
}
}
}
}

std::string file_contents(const std::string fname) {
  std::ifstream t(fname);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str() ;
}

t_type_kind t_type_case(const t_type& tt) {
  if (tt.__isset.base_type_val) return base_type_val ;
  if (tt.__isset.typedef_val) return typedef_val ;
  if (tt.__isset.enum_val) return enum_val ;
  if (tt.__isset.struct_val) return struct_val ;
  if (tt.__isset.xception_val) return xception_val ;
  if (tt.__isset.list_val) return list_val ;
  if (tt.__isset.set_val) return set_val ;
  if (tt.__isset.map_val) return map_val ;
  if (tt.__isset.service_val) return service_val ;
  throw apache::thrift::plugin::ThriftPluginError("Unknown t_type type");
  
}

void NiceJSON::json2binary(
			   const t_type_id id,
			   const json& jser,
			   ::apache::thrift::protocol::TProtocol* iprot) {

}
