#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <streambuf>
#include <string>
#include <vector>

#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TFileTransport.h>
#include "thrift/transport/TFDTransport.h"

#include "gen-cpp/plugin_types.h"

#include "json.hpp"

#ifndef NICEJSON_H_INCLUDED
#define NICEJSON_H_INCLUDED

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;
using std::set;
using std::vector;
using nlohmann::json;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::plugin ;

namespace apache {
namespace thrift {

template <typename ThriftStruct>
boost::shared_ptr<TMemoryBuffer> marshalToMemoryBuffer(const ThriftStruct& ts) {
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  boost::shared_ptr<TMemoryBuffer> trans(new TMemoryBuffer());
  TBinaryProtocol protocol(trans);

  ts.write(&protocol);
  return trans ;
}

template <typename ThriftStruct>
std::string ThriftBinaryString(const ThriftStruct& ts) {
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  TMemoryBuffer* buffer = new TMemoryBuffer;
  boost::shared_ptr<TTransport> trans(buffer);
  TBinaryProtocol protocol(trans);

  ts.write(&protocol);

  uint8_t* buf;
  uint32_t size;
  buffer->getBuffer(&buf, &size);
  return std::string((char*)buf, (unsigned int)size);
}

template <typename ThriftStruct>
std::string ThriftBinaryFramedString(const ThriftStruct& ts) {
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  TMemoryBuffer* buffer = new TMemoryBuffer;
  boost::shared_ptr<TTransport> trans(buffer);
  boost::shared_ptr<TTransport> framed(new TFramedTransport(trans)) ;
  TBinaryProtocol protocol(framed);

  ts.write(&protocol);
  framed->flush() ;

  uint8_t* buf;
  uint32_t size;
  buffer->getBuffer(&buf, &size);
  return std::string((char*)buf, (unsigned int)size);
}

}
}


namespace apache {
namespace thrift {
namespace nicejson {

struct NiceJSONError : public apache::thrift::TException {
  NiceJSONError(const std::string& msg) : apache::thrift::TException(msg) {}
};

struct t_struct_lookaside {
  t_type_id type_id ;
  std::string fqcppname ;
  t_struct st ;
  map<string, t_field> byname ;
  map<int32_t, t_field> byid ;
} ;

struct t_enum_lookaside {
  t_type_id type_id ;
  t_enum e ;
  map<string, int32_t> byname ;
  map<int32_t, string> byi32 ;
} ;

enum t_type_kind {
  base_type_val,
  typedef_val,
  enum_val,
  struct_val,
  xception_val,
  list_val,
  set_val,
  map_val,
  service_val,
} ;

t_type_kind t_type_case(const t_type& tt) ;

class NiceJSON {
public:
  NiceJSON(uint8_t const  * const serialized, size_t length) {
    deserialize(serialized, length) ;
    initialize() ;
  }

 NiceJSON(const std::string& idl) {
   deserialize((uint8_t const  * const)idl.data(), idl.length());
    initialize() ;
  }

 NiceJSON(const apache::thrift::plugin::GeneratorInput& x) : x_(x) {
   initialize() ;
 }

 private:
  void deserialize(uint8_t const  * const serialized, size_t length) {
    shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
    shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
    buf->resetBuffer(const_cast<uint8_t *>(serialized), static_cast<uint32_t>(length), TMemoryBuffer::COPY);
    x_.read(p.get());
  }

  void initialize() ;
  std::string program_cpp_prefix(const t_program_id program_id, const std::string& name) ;
  void add_service_lookasides(t_service_id dst, t_service_id svc) ;
  void add_struct_lookaside(const t_type_id id, const std::string& fqcppname, const t_struct& ts) ;

 public:
  void json2protocol(const t_type_id id, const t_type& tt, const json& jser, ::apache::thrift::protocol::TProtocol* oprot) const ;

  template <typename DST>
    void demarshal(const string name, const json& jser, DST *dst) const {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    boost::shared_ptr<TTransport> trans(new TMemoryBuffer());
    TBinaryProtocol protocol(trans);
    json2protocol(id, ty, jser, &protocol) ;
    dst->read(&protocol) ;
  }

  json skip2json(const ::apache::thrift::protocol::TType ftype, ::apache::thrift::protocol::TProtocol* iprot) const ;

  json protocol2json(const t_type_id id, const t_type& tt, ::apache::thrift::protocol::TProtocol* iprot, const bool permissive = false) const ;

  template <typename SRC>
    json marshal(const string name, const SRC& src) const {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    boost::shared_ptr<TTransport> trans(new TMemoryBuffer());
    TBinaryProtocol protocol(trans);
    src.write(&protocol) ;
    return protocol2json(id, ty, &protocol, false) ;
  }

  json marshal_from_binary(const string name, const uint8_t *serialized, const size_t length, const bool permissive = false) const {
    boost::shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
    buf->resetBuffer(const_cast<uint8_t *>(serialized), static_cast<uint32_t>(length), TMemoryBuffer::COPY);
    return marshal_from_binary(name, buf, permissive) ;
  }

  json marshal_from_binary(const string name, boost::shared_ptr<TMemoryBuffer> buf, const bool permissive = false) const {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    TBinaryProtocol protocol(buf);
    return protocol2json(id, ty, &protocol, permissive) ;
  }

  std::string demarshal_to_binary(const string name, const json& jser) const {
    TMemoryBuffer* buffer = new TMemoryBuffer;
    boost::shared_ptr<TTransport> trans(buffer);
    boost::shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(trans));

    demarshal_to_binary(name, jser, protocol) ;
    
    uint8_t* buf;
    uint32_t size;
    buffer->getBuffer(&buf, &size);
    return std::string((char*)buf, (unsigned int)size);
  }

  void demarshal_to_binary(const string name, const json& jser, boost::shared_ptr<TBinaryProtocol>& proto) const {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    json2protocol(id, ty, jser, proto.get()) ;
  }

  const apache::thrift::plugin::GeneratorInput& it() const { return x_ ; }

  t_type_id real_type_id(const t_type_id id) const {
    {
      auto ii = xtra_types.find(id) ;
      if (ii != xtra_types.end()) return id ;
    }

    const map<t_type_id, t_type>& types = it().type_registry.types ;
    const map<t_type_id, t_type>::const_iterator ii = types.find(id);
    assert(ii != types.end()) ;
    if (ii->second.__isset.typedef_val) {
      return real_type_id(ii->second.typedef_val.type) ;
    } else {
      return id ;
    }
  }

  const t_type& lookup_type(const t_type_id id) const {
    {
      auto ii = xtra_types.find(id) ;
      if (ii != xtra_types.end()) return ii->second ;
    }

    t_type_id real_id = real_type_id(id) ;

    const map<t_type_id, t_type>& types = it().type_registry.types ;
    const map<t_type_id, t_type>::const_iterator ii = types.find(real_id);
    assert(ii != types.end()) ;
    if (ii->second.__isset.typedef_val) {
      return lookup_type(ii->second.typedef_val.type) ;
    } else {
      return ii->second ;
    }
  }

  t_type_id lookup_type_id(const string& name) const {
    const map<string, t_type_id>::const_iterator ii = structs_by_name.find(name) ;
    assert (ii != structs_by_name.end()) ;
    return ii->second ;
  }

  const t_struct_lookaside& lookup_struct_fields(const t_type_id id) const {
    t_type_id real_id = real_type_id(id) ;

    const map<t_type_id, t_struct_lookaside>::const_iterator ii = struct_lookaside.find(real_id);
    assert(ii != struct_lookaside.end()) ;
    return ii->second ;
  }

  const t_enum_lookaside& lookup_enum(const t_type_id id) const {
    t_type_id real_id = real_type_id(id) ;

    const map<t_type_id, t_enum_lookaside>::const_iterator ii = enum_lookaside.find(real_id);
    assert(ii != enum_lookaside.end()) ;
    return ii->second ;
  }

  const std::pair<std::string, std::string>& service_struct_names(const std::string& service, const std::string& operation) const {
    auto aa = service_lookaside_.find(service) ;
    if (aa == service_lookaside_.end()) {
      throw NiceJSONError("unknown service") ;
    }
    auto bb = aa->second.find(operation) ;
    if (bb == aa->second.end()) {
      throw NiceJSONError("unknown operation") ;
    }
    return bb->second ;
  }

  static void register_typelib(const string& package, const string& name, const NiceJSON *p) ;
  static void register_typelib(const string& key, NiceJSON const * const p) ;
  static NiceJSON const * const install_typelib(const string& package, const string& name, const string& serialized) ;
  static NiceJSON const * const install_typelib(const string& key, const string& serialized) ;

  static const NiceJSON* lookup_typelib(const string& key) ;
  static bool typelib_installed(const string& key) ;
  static NiceJSON const * const require_typelib(const string& typelib) ;

  static void prepend_typelib_directory(const std::string& dir) ;
  static void append_typelib_directory(const std::string& dir) ;

  typedef map<string, const NiceJSON *> type_library_t;

  apache::thrift::plugin::GeneratorInput x_;
  t_program_id program_id_ ;
  map<t_program_id, t_program> program_lookaside_ ;
  map<t_type_id, t_struct_lookaside> struct_lookaside ;
  map<string, t_type_id> structs_by_name ;
  map<t_type_id, t_enum_lookaside> enum_lookaside ;
  map<t_type_id, t_type> xtra_types;
  map<std::string, map<std::string, std::pair<std::string, std::string> > > service_lookaside_ ;

  static type_library_t *type_library_() {
    static type_library_t *type_library = new type_library_t() ;
    return type_library ;
  }

  static vector<string> typelib_path_prepends ;
  static vector<string> typelib_path_appends ;
} ;

std::string file_contents(const std::string fname) ;
std::string hex(const std::string& v) ;
void f_write(const std::string& fname, const std::string& b) ;

}
}
}
#endif
