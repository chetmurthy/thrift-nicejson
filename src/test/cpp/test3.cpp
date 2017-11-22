#include <fstream>
#include <iostream>
#include <iostream>
#include <streambuf>
#include <string>
#include <tuple>

#define BOOST_TEST_MODULE NiceJSONRPCTest
#include <boost/smart_ptr.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/thread.hpp>

#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TFileTransport.h>
#include "thrift/transport/TFDTransport.h"
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/server/TThreadedServer.h>

#include "gen-cpp/test_types.h"
#include "gen-cpp-typelib/test_typelib.h"
#include "gen-cpp/S2.h"

#include "TNiceJSONProtocol.h"
#include "NiceJSON.h"

using boost::shared_ptr;
using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::map;
using std::list;
using std::set;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::nicejson;

BOOST_AUTO_TEST_CASE( mt )
{
}

const std::string kTutTypelib = "tutorial.tutorial" ;

bool thrift_test::Bar::operator<(thrift_test::Bar const& that) const {
  if (this->a != that.a) return this->a < that.a ;
  if (this->b != that.b) return this->b < that.b ;
  return false ;
}

class S2Handler : public thrift_test::S2If {
public:
  S2Handler() {}

  void ping() override { cout << "ping()" << endl; }
  int32_t foo(const int32_t logid, const thrift_test::Bar& w) override {
    cout << "foo(" << logid << ", ...)" << endl;
    return logid;
  }
  void goo(thrift_test::Bar& bar) override {
  }
};

class S2CloneFactory : virtual public thrift_test::S2IfFactory {
 public:
  virtual ~S2CloneFactory() {}
  virtual thrift_test::S2If* getHandler(const ::apache::thrift::TConnectionInfo& connInfo)
  {
    boost::shared_ptr<TSocket> sock = boost::dynamic_pointer_cast<TSocket>(connInfo.transport);
    cout << "Incoming connection\n";
    cout << "\tSocketInfo: "  << sock->getSocketInfo() << "\n";
    cout << "\tPeerHost: "    << sock->getPeerHost() << "\n";
    cout << "\tPeerAddress: " << sock->getPeerAddress() << "\n";
    cout << "\tPeerPort: "    << sock->getPeerPort() << "\n";
    return new S2Handler;
  }
  virtual void releaseHandler( thrift_test::S2If* handler) {
    delete handler;
  }
};

class RPC0ThreadClass {
public:
  RPC0ThreadClass(TThreadedServer& server) : server_(server) { } // Constructor
~RPC0ThreadClass() { } // Destructor

void Run() {
  server_.serve() ;
}
 TThreadedServer& server_ ;
} ;

void client() {
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
  thrift_test::S2Client client(protocol);

  try {
    transport->open();

    client.ping();

    int32_t rv = client.foo(42l, thrift_test::Bar()) ;
    BOOST_CHECK( rv == 42l ) ;

    thrift_test::Bar b ;
    client.goo(b) ;

    transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
}

BOOST_AUTO_TEST_CASE( RPC0 )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9090), //port
    boost::make_shared<TBufferedTransportFactory>(),
    boost::make_shared<TBinaryProtocolFactory>());

  cout << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  cout << "Done." << endl;
  client() ;
  server.stop();
  thread.join() ;
}

