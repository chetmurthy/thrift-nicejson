#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <streambuf>
#include <iostream>

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

template<class T>
class NiceJSON {
public:
  NiceJSON(const std::string& idl) {
    shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
    shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
    buf->resetBuffer((uint8_t*)idl.data(), static_cast<uint32_t>(idl.length()));
    x_.read(p.get());
  }

  NiceJSON(boost::shared_ptr<TTransport> transport) {
    TJSONProtocol proto(transport);

    x_.read(&proto);

    auto alltypes = it().type_registry.types ;
    for(map<t_type_id, t_type>::const_iterator ii = alltypes.begin() ; ii != alltypes.end(); ++ii) {
      const t_type_id id = ii->first ;
      const t_type& ty = ii->second ;

      if (ty.__isset.struct_val) {
	const t_struct &st = ty.struct_val ;
	struct2type[st.metadata.name] = t_struct_fields() ;
	t_struct_fields& p = struct2type[st.metadata.name] ;

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

  const T& it() { return x_ ; }
  const t_type& lookup_type(const t_type_id id) { return it().type_registry.types[id] ; }

private:
  T x_;
  map<string, t_struct_fields> struct2type ;
} ;

std::string file_contents(const std::string fname) ;

