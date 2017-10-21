#include <string>
#include <fstream>
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
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

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
  }

  const T& it() { return x_ ; }

private:
  T x_;
} ;

std::string file_contents(const std::string fname) ;

