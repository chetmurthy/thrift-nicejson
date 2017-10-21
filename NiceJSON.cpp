
#include "NiceJSON.h"

std::string file_contents(const std::string fname) {
  std::ifstream t(fname);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str() ;
}

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
