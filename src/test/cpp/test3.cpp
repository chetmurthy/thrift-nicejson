#include <exception>
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
#include <thrift/transport/THttpServer.h>
#include <thrift/transport/THttpClient.h>
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
using std::cerr;
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

const std::string kTestTypelib = "thrift_test.test" ;

bool thrift_test::Bar::operator<(thrift_test::Bar const& that) const {
  if (this->a != that.a) return this->a < that.a ;
  if (this->b != that.b) return this->b < that.b ;
  return false ;
}

class myexception: public std::exception
{
  virtual const char* what() const throw()
  {
    return "My exception happened";
  }
};

class S2Handler : public thrift_test::S2If {
public:
  S2Handler() {}

  void ping() override { cerr << "ping()" << endl; }
  int32_t foo(const int32_t n, const thrift_test::Bar& w) override {
    cerr << "foo(" << n << ", ...)" << endl;
    switch (n) {
    case 0: {
      thrift_test::InvalidOperation e ;
      e.whatOp = 84 ;
      e.why = "argle" ;
      throw e ;
    }
    case 1: {
      thrift_test::InvalidOperation2 e ;
      e.whatOp = 42 ;
      e.why = "gurgle" ;
      throw e ;
    }
    case 2: {
      throw myexception() ;
    }
    default:
      return n ;
    }
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
    cerr << "Incoming connection\n";
    cerr << "\tSocketInfo: "  << sock->getSocketInfo() << "\n";
    cerr << "\tPeerHost: "    << sock->getPeerHost() << "\n";
    cerr << "\tPeerAddress: " << sock->getPeerAddress() << "\n";
    cerr << "\tPeerPort: "    << sock->getPeerPort() << "\n";
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

void base_client(thrift_test::S2Client& client) {
  cerr << "Starting the client..." << endl;

  client.ping();

  BOOST_CHECK( client.foo(42l, thrift_test::Bar()) == 42l ) ;

  thrift_test::Bar b ;
  client.goo(b) ;

  BOOST_CHECK_THROW( client.foo(0l, thrift_test::Bar()),
		     thrift_test::InvalidOperation ) ;
  BOOST_CHECK_THROW( client.foo(1l, thrift_test::Bar()),
		     thrift_test::InvalidOperation2 ) ;
  BOOST_CHECK_THROW( client.foo(2l, thrift_test::Bar()),
		     apache::thrift::TApplicationException ) ;

}

BOOST_AUTO_TEST_CASE( RPC0 )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9090), //port
    boost::make_shared<TBufferedTransportFactory>(),
    boost::make_shared<TBinaryProtocolFactory>());

  cerr << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  sleep(1) ;
  {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    thrift_test::S2Client client(protocol);

    transport->open();
    base_client(client) ;
    transport->close();
  }
  server.stop();
  thread.join() ;
}

BOOST_AUTO_TEST_CASE( RPC1 )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9091), //port
    boost::make_shared<TBufferedTransportFactory>(),
    boost::make_shared<TNiceJSONProtocolFactory>(kTestTypelib, "S2"));

  cerr << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  sleep(1) ;
  {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9091));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TNiceJSONProtocol(kTestTypelib, "S2", transport));
    thrift_test::S2Client client(protocol);

    transport->open();
    base_client(client) ;
    transport->close();
  }
  server.stop();
  thread.join() ;
}

BOOST_AUTO_TEST_CASE( RPC2 )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9092), //port
    boost::make_shared<THttpServerTransportFactory>(),
    boost::make_shared<TBinaryProtocolFactory>());

  cerr << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  sleep(1) ;
  {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9092));
    boost::shared_ptr<TTransport> transport(new THttpClient(socket, "localhost", "/service"));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    thrift_test::S2Client client(protocol);

    transport->open();
    base_client(client) ;
    transport->close();
  }
  server.stop();
  thread.join() ;
}

BOOST_AUTO_TEST_CASE( RPC2b )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9092), //port
    boost::make_shared<THttpServerTransportFactory>(),
    boost::make_shared<TJSONProtocolFactory>());

  cerr << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  sleep(1) ;
  {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9092));
    boost::shared_ptr<TTransport> transport(new THttpClient(socket, "localhost", "/service"));
    boost::shared_ptr<TProtocol> protocol(new TJSONProtocol(transport));
    thrift_test::S2Client client(protocol);

    transport->open();
    base_client(client) ;
    transport->close();
  }
  server.stop();
  thread.join() ;
}

BOOST_AUTO_TEST_CASE( RPC3 )
{
  TThreadedServer server(
    boost::make_shared<thrift_test::S2ProcessorFactory>(boost::make_shared<S2CloneFactory>()),
    boost::make_shared<TServerSocket>(9092), //port
    boost::make_shared<THttpServerTransportFactory>(),
    boost::make_shared<TNiceJSONProtocolFactory>(kTestTypelib, "S2"));

  cerr << "Starting the server..." << endl;
  RPC0ThreadClass t(server) ;
  boost::thread thread(&RPC0ThreadClass::Run, &t);

  sleep(1) ;
  {
    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9092));
    boost::shared_ptr<TTransport> transport(new THttpClient(socket, "localhost", "/service"));
    boost::shared_ptr<TProtocol> protocol(new TNiceJSONProtocol(kTestTypelib, "S2", transport));
    thrift_test::S2Client client(protocol);

    transport->open();
    base_client(client) ;
    transport->close();
  }
  server.stop();
  thread.join() ;
}
