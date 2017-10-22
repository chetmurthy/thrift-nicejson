
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

TType t_base2ttype(const t_base::type& bt) {
  switch(bt) {
  case t_base::TYPE_VOID:
    return T_VOID ;

  case t_base::TYPE_STRING:
    return T_STRING ;
    
  case t_base::TYPE_BOOL:
    return T_BOOL ;
    
  case t_base::TYPE_I8:
    return T_I08 ;
    
  case t_base::TYPE_I16:
    return T_I16 ;
    
  case t_base::TYPE_I32:
    return T_I32 ;
    
  case t_base::TYPE_I64:
    return T_I64 ;
    
  case t_base::TYPE_DOUBLE:
    return T_DOUBLE ;
    
  case t_base::TYPE_BINARY:
    throw apache::thrift::plugin::ThriftPluginError("t_base2ttype: unhandled case TYPE_BINARY");
  default:
    throw apache::thrift::plugin::ThriftPluginError("t_base2ttype: Unknown base_type type");
  }
}

TType t_type2ttype(const t_type& tt) {
  switch(t_type_case(tt)) {
  case base_type_val:
    return t_base2ttype(tt.base_type_val.value) ;
  default:
    throw apache::thrift::plugin::ThriftPluginError("t_type2ttype: Unknown t_type type");
  }
}

void NiceJSON::json2protocol(
			   const t_type_id id,
			   const json& jser,
			   ::apache::thrift::protocol::TProtocol* oprot) {
  const t_type& tt = lookup_type(id) ;
  switch (t_type_case(tt)) {
  case struct_val: {
    /*
     * (1) lookup struct info
     * (2) check that the JSON is an object
     * (3) begin struct
     * (4) for each member of JSON object, look up field, and if it's there,
     *     begin field/ recurse / end field
     * (5) end struct
     */
    const t_struct_fields& fdata = lookup_struct_fields(id) ;
    if (!jser.is_object()) 
      throw apache::thrift::plugin::ThriftPluginError("cannot deser struct from non-object JSON");
    oprot->writeStructBegin(fdata.st.metadata.name.data()) ;
    for(json::const_iterator fii = jser.begin() ; fii != jser.end() ; ++fii) {
      const string& fname = fii.key() ;
      const json& fvalue = fii.value() ;
      map<string, t_field>::const_iterator sfii = fdata.byname.find(fname) ;
      if (sfii == fdata.byname.end()) continue ;
      const t_field& f = sfii->second ;
      const t_type& ftype = lookup_type(f.type) ;
      const TType ttype = t_type2ttype(ftype) ;
      oprot->writeFieldBegin(f.name.data(), ttype, f.key) ;
      json2protocol(f.type, fvalue, oprot) ;
      oprot->writeFieldEnd() ;
    }
    oprot->writeFieldStop();
    oprot->writeStructEnd() ;
    break ;
  }

  case base_type_val: {
    switch (t_base2ttype(tt.base_type_val.value)) {
    case T_I32: {
      if (!jser.is_number())
	throw apache::thrift::plugin::ThriftPluginError("json2protocol: non-numeric member");
      int n = jser.get<int>() ;
      oprot->writeI32(n) ;
      break ;
    }
      
    case T_STRING: {
      if (!jser.is_string())
	throw apache::thrift::plugin::ThriftPluginError("json2protocol: non-string member");
      string s = jser.get<string>() ;
      oprot->writeString(s) ;
      break ;
    }
    default:
      throw apache::thrift::plugin::ThriftPluginError("unhandled t_base");
    }
    break ;
  }

  default:
    throw apache::thrift::plugin::ThriftPluginError("unhandled t_type");
  }
}
