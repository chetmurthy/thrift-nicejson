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

struct t_struct_fields {
  t_type_id type_id ;
  t_struct st ;
  map<string, t_field> byname ;
  map<int32_t, t_field> byid ;
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
  NiceJSON(const std::string& idl) {
    shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
    shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
    buf->resetBuffer((uint8_t*)idl.data(), static_cast<uint32_t>(idl.length()));
    x_.read(p.get());
    initialize() ;
  }

  NiceJSON(boost::shared_ptr<TTransport> transport) {
    TJSONProtocol proto(transport);

    x_.read(&proto);
    initialize() ;
  }

  void initialize() {
    auto alltypes = it().type_registry.types ;
    for(map<t_type_id, t_type>::const_iterator ii = alltypes.begin() ; ii != alltypes.end(); ++ii) {
      const t_type_id id = ii->first ;
      const t_type& ty = ii->second ;

      if (ty.__isset.struct_val) {
	structs_by_name[ty.struct_val.metadata.name] = id ;

	struct2type[id] = t_struct_fields() ;
	t_struct_fields& p = struct2type[id] ;

	p.type_id = id ;
	p.st = ty.struct_val ;
	for(vector<t_field>::const_iterator jj = p.st.members.begin() ; jj != p.st.members.end() ; ++jj) {
	  const t_field& f = *jj ;
	  p.byname[f.name] = f ;
	  p.byid[f.key] = f ;
	}
      }
    }
  }

  void json2protocol(const t_type_id id, const t_type& tt, const json& jser, ::apache::thrift::protocol::TProtocol* oprot) ;

  template <typename DST>
    void demarshal(const string name, const json& jser, DST *dst) {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    boost::shared_ptr<TTransport> trans(new TMemoryBuffer());
    TBinaryProtocol protocol(trans);
    json2protocol(id, ty, jser, &protocol) ;
    dst->read(&protocol) ;
  }

  json protocol2json(const t_type_id id, const t_type& tt, ::apache::thrift::protocol::TProtocol* iprot) ;

  template <typename SRC>
    json marshal(const string name, const SRC& src) {
    const t_type_id id = lookup_type_id(name) ;
    const t_type& ty = lookup_type(id) ;
    boost::shared_ptr<TTransport> trans(new TMemoryBuffer());
    TBinaryProtocol protocol(trans);
    src.write(&protocol) ;
    return protocol2json(id, ty, &protocol) ;
  }

  const apache::thrift::plugin::GeneratorInput& it() const { return x_ ; }
  const t_type& lookup_type(const t_type_id id) const {
    const map<t_type_id, t_type>& types = it().type_registry.types ;
    const map<t_type_id, t_type>::const_iterator ii = types.find(id);
    assert(ii != types.end()) ;
    if (ii->second.__isset.typedef_val) {
      return lookup_type(ii->second.typedef_val.type) ;
    } else {
      return ii->second ;
    }
  }

  const t_type_id lookup_type_id(const string& name) const {
    const map<string, t_type_id>::const_iterator ii = structs_by_name.find(name) ;
    assert (ii != structs_by_name.end()) ;
    return ii->second ;
  }

  const t_struct_fields& lookup_struct_fields(const t_type_id id) const {
    const map<t_type_id, t_struct_fields>::const_iterator ii = struct2type.find(id);
    assert(ii != struct2type.end()) ;
    return ii->second ;
  }

private:
  apache::thrift::plugin::GeneratorInput x_;
  map<t_type_id, t_struct_fields> struct2type ;
  map<string, t_type_id> structs_by_name ;
} ;

std::string file_contents(const std::string fname) ;

