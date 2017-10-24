
#include "NiceJSON.h"


namespace apache {
namespace thrift {
namespace nicejson {

struct NiceJSONError : public apache::thrift::TException {
  NiceJSONError(const std::string& msg) : apache::thrift::TException(msg) {}
};

}
}
}

namespace apache {
namespace thrift {
namespace plugin {

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
  throw apache::thrift::nicejson::NiceJSONError("Unknown const value type");
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
  throw apache::thrift::nicejson::NiceJSONError("Unknown const value type");
}
}
}
}

namespace apache {
namespace thrift {
namespace nicejson {

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
  throw NiceJSONError("t_type_case: Unknown t_type type");
  
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
    throw NiceJSONError("t_base2ttype: unhandled case TYPE_BINARY");
  default:
    throw NiceJSONError("t_base2ttype: Unknown base_type type");
  }
}

TType t_type2ttype(const t_type& tt) {
  switch(t_type_case(tt)) {
  case base_type_val:
    return t_base2ttype(tt.base_type_val.value) ;
  case list_val:
    return T_LIST ;
  case set_val:
    return T_SET ;
  case struct_val:
    return T_STRUCT ;
  case map_val:
    return T_MAP ;
  case enum_val:
    return T_I32 ;
  default:
    std::cerr << boost::format{"t_type2ttype: Unknown t_type type %d, %s"} % t_type_case(tt) % apache::thrift::ThriftDebugString(tt) << endl ;
    throw NiceJSONError("t_type2ttype: Unknown t_type type");
  }
}

template <typename ITY>
ITY get_exactly(const json& j) {
  if (!j.is_number()) 
    throw NiceJSONError("json2protocol: non-numeric");
  double d = j.get<double>() ;
  int64_t rv = (ITY) d ;
  double dd = (double) rv ;
  if (dd != d)
    throw NiceJSONError("json2protocol: numeric but inexact/non-integral");
  if (rv < (int64_t)std::numeric_limits<ITY>::min() || (int64_t)std::numeric_limits<ITY>::max() < rv)
    throw NiceJSONError("json2protocol: numeric but out-of-bounds");
  return (ITY)rv ;
}

void NiceJSON::json2protocol(
			     const t_type_id id,
			     const t_type& tt,
			     const json& jser,
			     ::apache::thrift::protocol::TProtocol* oprot) const {
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
    const t_struct_lookaside& fdata = lookup_struct_fields(id) ;
    if (!jser.is_object()) 
      throw NiceJSONError("cannot deser struct from non-object JSON");
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
      json2protocol(f.type, ftype, fvalue, oprot) ;
      oprot->writeFieldEnd() ;
    }
    oprot->writeFieldStop();
    oprot->writeStructEnd() ;
    break ;
  }

  case enum_val: {
    if (!jser.is_string())
      throw NiceJSONError("json2protocol: not a string");

    const t_enum_lookaside& ee = lookup_enum(id) ;
    string s = jser.get<string>() ;
    map<string, int32_t>::const_iterator ii = ee.byname.find(s) ;
    if (ii == ee.byname.end())
      throw NiceJSONError("json2protocol: unrecognized enum string");
    oprot->writeI32(ii->second) ;
    break ;
  }

  case base_type_val: {
    switch (t_base2ttype(tt.base_type_val.value)) {
    case T_I08: {
      int8_t n = get_exactly<int8_t>(jser) ;
      oprot->writeByte((int8_t)n) ;
      break ;
    }
    case T_I16: {
      int16_t n = get_exactly<int16_t>(jser) ;
      oprot->writeI16(n) ;
      break ;
    }
    case T_I32: {
      int32_t n = get_exactly<int32_t>(jser) ;
      oprot->writeI32(n) ;
      break ;
    }
    case T_I64: {
      if (!jser.is_string())
	throw NiceJSONError("json2protocol: non-string member");
      string s = jser.get<string>() ;
      int64_t n = std::stoll(s) ;
      oprot->writeI64(n) ;
      break ;
    }

    case T_BOOL: {
      if (!jser.is_boolean())
	throw NiceJSONError("json2protocol: non-boolean member");
      bool b = jser.get<bool>() ;
      oprot->writeBool(b) ;
      break ;
    }
      
    case T_STRING: {
      if (!jser.is_string())
	throw NiceJSONError("json2protocol: non-string member");
      string s = jser.get<string>() ;
      oprot->writeString(s) ;
      break ;
    }

    default:
      std::cerr << boost::format{"json2protocol: unhandled t_base %s"} % tt << std::endl ;
      throw NiceJSONError("json2protocol: unhandled t_base");
    }
    break ;
  }

  case list_val: {
    if (!jser.is_array())
	throw NiceJSONError("json2protocol: not an array");
    const uint32_t size = jser.size() ;
    const t_type_id elem_type_id = tt.list_val.elem_type ;
    const t_type& elem_type = lookup_type(elem_type_id) ;
    const TType elemTType = t_type2ttype(elem_type) ;
    oprot->writeListBegin(elemTType, size) ;
    for(json::const_iterator aii = jser.begin() ; aii != jser.end() ; ++aii) {
      const json& elem = *aii ;
      json2protocol(elem_type_id, elem_type, elem, oprot) ;
    }
    oprot->writeListEnd() ;
    break ;
  }

  case set_val: {
    if (!jser.is_array())
	throw NiceJSONError("json2protocol: not an array");
    const uint32_t size = jser.size() ;
    const t_type_id elem_type_id = tt.set_val.elem_type ;
    const t_type& elem_type = lookup_type(elem_type_id) ;
    const TType elemTType = t_type2ttype(elem_type) ;
    oprot->writeSetBegin(elemTType, size) ;
    for(json::const_iterator aii = jser.begin() ; aii != jser.end() ; ++aii) {
      const json& elem = *aii ;
      json2protocol(elem_type_id, elem_type, elem, oprot) ;
    }
    oprot->writeSetEnd() ;
    break ;
  }

  case map_val: {
    const t_type_id dom_type_id = tt.map_val.key_type ;
    const t_type& dom_type = lookup_type(dom_type_id) ;
    const TType domTType = t_type2ttype(dom_type) ;

    const t_type_id rng_type_id = tt.map_val.val_type ;
    const t_type& rng_type = lookup_type(rng_type_id) ;
    const TType rngTType = t_type2ttype(rng_type) ;

    if (domTType == T_STRING) {
      if (!jser.is_object())
	throw NiceJSONError("json2protocol: not an object");
      const uint32_t size = jser.size() ;
      oprot->writeMapBegin(domTType, rngTType, size) ;
      for(json::const_iterator aii = jser.begin() ; aii != jser.end() ; ++aii) {
	const json& key = aii.key() ;
	const json& value = aii.value() ;
	if (!key.is_string())
	  throw NiceJSONError("json2protocol: key is not a string");
	json2protocol(dom_type_id, dom_type, key, oprot) ;
	json2protocol(rng_type_id, rng_type, value, oprot) ;
      }
    }
    else {
      if (!jser.is_array())
	throw NiceJSONError("json2protocol: not an array");
      const uint32_t size = jser.size() ;
      oprot->writeMapBegin(domTType, rngTType, size) ;
      for(json::const_iterator aii = jser.begin() ; aii != jser.end() ; ++aii) {
	const json& key = (*aii)[0] ;
	const json& value = (*aii)[1] ;
	json2protocol(dom_type_id, dom_type, key, oprot) ;
	json2protocol(rng_type_id, rng_type, value, oprot) ;
      }
    }
    oprot->writeMapEnd() ;
    break ;
  }

  default:
    std::cerr << boost::format{"json2protocol: unhandled t_type %s"} % tt << std::endl ;
    throw NiceJSONError("json2protocol: unhandled t_type");
  }
}

json NiceJSON::protocol2json(const t_type_id id,
			     const t_type& tt,
			     ::apache::thrift::protocol::TProtocol* iprot,
			     const bool permissive) const {
  switch (t_type_case(tt)) {
  case struct_val: {
    /*
     * (1) lookup struct info
     * (2) begin struct
     * (3) for each beginField (until we get T_STOP),  recurse / end field, addinng to JSON
     *     begin field/ recurse / end field
     * (4) end struct
     */
    json rv = "{}"_json ;

    const t_struct_lookaside& fdata = lookup_struct_fields(id) ;
    string sname ;
    iprot->readStructBegin(sname) ;
    while (true) {
      std::string fname;
      ::apache::thrift::protocol::TType ftype;
      int16_t fid;
      iprot->readFieldBegin(fname, ftype, fid);
      if (ftype == ::apache::thrift::protocol::T_STOP) {
	break;
      }
      const map<int32_t, t_field>::const_iterator fii = fdata.byid.find(fid) ;
      if (fii == fdata.byid.end()) {
	if (permissive) {
	  rv[std::to_string(fid)] = skip2json(ftype, iprot) ;
	} else
	    iprot->skip(ftype) ;
      }
      else {
	const t_field& f = fii->second ;
	const t_type& f_type = lookup_type(f.type) ;
	const TType ttype = t_type2ttype(f_type) ;
	if (ttype != ftype) {
	  if (permissive) {
	    rv[std::to_string(fid)] = skip2json(ftype, iprot) ;
	  } else
	    iprot->skip(ftype) ;
	}
	else {
	  json fvalue = protocol2json(f.type, f_type, iprot, permissive) ;
	  rv[f.name] = fvalue ;
	}
      }
      iprot->readFieldEnd() ;
    }
    iprot->readStructEnd() ;
    return rv ;
  }

  case enum_val: {
    const t_enum_lookaside& ee = lookup_enum(id) ;
    int n;
    iprot->readI32(n) ;
    map<int32_t, string>::const_iterator ii = ee.byi32.find(n) ;
    if (ii == ee.byi32.end()) {
      throw NiceJSONError("protocol2json: unrecognized enum value");
    }
    json rv = ii->second ;
    return rv ;
  }

  case base_type_val: {
    switch (t_base2ttype(tt.base_type_val.value)) {
    case T_I08: {
      int8_t n;
      iprot->readByte(n) ;
      json rv = n ;
      return rv ;
    }
    case T_I16: {
      int16_t n;
      iprot->readI16(n) ;
      json rv = n ;
      return rv ;
    }
    case T_I32: {
      int n;
      iprot->readI32(n) ;
      json rv = n ;
      return rv ;
    }
    case T_I64: {
      int64_t n ;
      iprot->readI64(n) ;
      json rv = std::to_string(n) ;
      return rv ;
    }
    case T_DOUBLE: {
      double n ;
      iprot->readDouble(n) ;
      json rv = n ;
      return rv ;
    }
    case T_BOOL: {
      bool b;
      iprot->readBool(b) ;
      json rv = b ;
      return rv ;
    }
      
    case T_STRING: {
      string s ;
      iprot->readString(s) ;
      json rv = s ;
      return rv ;
    }
    default:
      std::cerr << boost::format{"protocol2json: unhandled t_base %s"} % apache::thrift::ThriftDebugString(tt.base_type_val) << std::endl ;
      throw NiceJSONError("protocol2json: unhandled t_base");
    }
  }

  case list_val: {
    json rv = "[]"_json ; // TODO: fix this to be more efficient maybe?
    uint32_t size ;
    ::apache::thrift::protocol::TType eTType ;
    const t_type_id elem_type_id = tt.list_val.elem_type ;
    const t_type& elem_type = lookup_type(elem_type_id) ;
    iprot->readListBegin(eTType, size) ;
    for(uint32_t i = 0 ; i < size ; ++i) {
      rv.push_back(protocol2json(elem_type_id, elem_type, iprot, permissive)) ;
    }
    iprot->readListEnd() ;
    return rv ;
  }

  case set_val: {
    json rv = "[]"_json ; // TODO: fix this to be more efficient maybe?
    uint32_t size ;
    ::apache::thrift::protocol::TType eTType ;
    const t_type_id elem_type_id = tt.set_val.elem_type ;
    const t_type& elem_type = lookup_type(elem_type_id) ;
    iprot->readSetBegin(eTType, size) ;
    for(uint32_t i = 0 ; i < size ; ++i) {
      rv.push_back(protocol2json(elem_type_id, elem_type, iprot, permissive)) ;
    }
    iprot->readSetEnd() ;
    return rv ;
  }

  case map_val: {
    uint32_t size ;
    ::apache::thrift::protocol::TType domTType ;
    ::apache::thrift::protocol::TType rngTType ;
    iprot->readMapBegin(domTType, rngTType, size) ;
    const t_type_id dom_type_id = tt.map_val.key_type ;
    const t_type& dom_type = lookup_type(dom_type_id) ;
    const t_type_id rng_type_id = tt.map_val.val_type ;
    const t_type& rng_type = lookup_type(rng_type_id) ;
    json rv ;
    if (domTType == T_STRING) {
      rv = "{}"_json ; // TODO: fix this to be more efficient maybe?
      for(uint32_t i = 0 ; i < size ; ++i) {
	json key = protocol2json(dom_type_id, dom_type, iprot, permissive) ;
	json value = protocol2json(rng_type_id, rng_type, iprot, permissive) ;
	rv[key.get<string>()] = value ;
      }
    }
    else {
      rv = "[]"_json ;
      for(uint32_t i = 0 ; i < size ; ++i) {
	json key = protocol2json(dom_type_id, dom_type, iprot, permissive) ;
	json value = protocol2json(rng_type_id, rng_type, iprot, permissive) ;
	rv.push_back({key, value}) ;
      }
    }
    iprot->readMapEnd() ;
    return rv ;
  }

  default:
    std::cerr << boost::format{"protocol2json: unhandled t_type %s"} % tt << std::endl ;
    throw NiceJSONError("protocol2json: unhandled t_type");
  }
}


json NiceJSON::skip2json(const ::apache::thrift::protocol::TType ftype, ::apache::thrift::protocol::TProtocol* iprot) const {
  switch (ftype) {
  case T_BOOL: {
    bool boolv;
    iprot->readBool(boolv);
    json rv = boolv ;
    return rv ;
  }
  case T_BYTE: {
    int8_t bytev;
    iprot->readByte(bytev);
    json rv = bytev ;
    return rv ;
  }
  case T_I16: {
    int16_t i16;
    iprot->readI16(i16);
    json rv = i16 ;
    return rv ;
  }
  case T_I32: {
    int32_t i32;
    iprot->readI32(i32);
    json rv = i32 ;
    return rv ;
  }
  case T_I64: {
    int64_t i64;
    iprot->readI64(i64);
    json rv = std::to_string(i64) ;
    return rv ;
  }
  case T_DOUBLE: {
    double dub;
    iprot->readDouble(dub);
    json rv = dub ;
    return rv ;
  }
  case T_STRING: {
    std::string str;
    iprot->readBinary(str);
    json rv = str ;
    return rv ;
  }
  case T_STRUCT: {
    std::string name;
    int16_t fid;
    TType ftype;
    iprot->readStructBegin(name);
    json j = "{}"_json ;
    while (true) {
      iprot->readFieldBegin(name, ftype, fid);
      if (ftype == T_STOP) {
	break;
      }
      j[std::to_string(fid)] = skip2json(ftype, iprot);
      iprot->readFieldEnd();
    }
    iprot->readStructEnd();
    return j ;
  }
  case T_MAP: {
    TType keyType;
    TType valType;
    uint32_t i, size;
    iprot->readMapBegin(keyType, valType, size);
    json j = "[]"_json ;
    for (i = 0; i < size; i++) {
      json k = skip2json(keyType, iprot);
      json v = skip2json(valType, iprot);
      j.push_back({ k, v }) ;
    }
    iprot->readMapEnd();
    return j ;
  }
  case T_SET: {
    TType elemType;
    uint32_t i, size;
    iprot->readSetBegin(elemType, size);
    json j = "[]"_json ;
    for (i = 0; i < size; i++) {
      j.push_back(skip2json(elemType, iprot));
    }
    iprot->readSetEnd();
    return j ;
  }
  case T_LIST: {
    TType elemType;
    uint32_t i, size;
    iprot->readListBegin(elemType, size);
    json j = "[]"_json ;
    for (i = 0; i < size; i++) {
      j.push_back(skip2json(elemType, iprot));
    }
    iprot->readListEnd();
    return j;
  }
  case T_STOP:
  case T_VOID:
  case T_U64:
  case T_UTF8:
  case T_UTF16:
    break;
  }
  json j;
  return j;    
}

map<string, const NiceJSON*>  NiceJSON::type_library_;

void NiceJSON::register_typelib(const string& package, const string& name, const NiceJSON *p) {
  const string key = package + "/" + name ;
  if (type_library_.end() != type_library_.find(key)) {
    throw NiceJSONError("Type library " + key + " already registered") ;
  }
  type_library_[key] = p ;
}

const NiceJSON* NiceJSON::lookup_typelib(const string& key) {
  if (type_library_.end() == type_library_.find(key)) {
    throw NiceJSONError("Type library " + key + " not found") ;
  }
  return type_library_[key] ;
}

}
}
}
