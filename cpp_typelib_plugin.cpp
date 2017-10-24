/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <iostream>

#include "boost/filesystem.hpp"
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>

#include "thrift/generate/t_generator.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include "thrift/transport/TFDTransport.h"

#include "thrift/plugin/plugin.h"
#include "gen-cpp/plugin_types.h"

using namespace std ;
using namespace boost::filesystem;
using apache::thrift::protocol::TBinaryProtocol;
using apache::thrift::transport::TFDTransport;
using apache::thrift::transport::TFramedTransport;

int main(int argc, char* argv[]) {
    boost::shared_ptr<TFramedTransport> transport(
      new TFramedTransport(boost::make_shared<TFDTransport>(fileno(stdin))));
  TBinaryProtocol proto(transport);
  apache::thrift::plugin::GeneratorInput input;
  try {
    input.read(&proto);
  } catch (std::exception& err) {
    std::cerr << "Error while receiving plugin data: " << err.what() << std::endl;
    exit(-1);
  }

  cerr << "Generating typelib" << endl ;

  string ns ;
  {
    map<string, string>::const_iterator ii = input.program.namespaces.find("typelib") ;
    if (ii == input.program.namespaces.end()) {
      std::cerr << "No (required) typelib namespace declared" << std::endl ;
      exit(-1) ;
    }
    ns = ii->second ;
  }

  string out_path = input.program.out_path ;
  string name = input.program.name ;
  path destdir(str(boost::format{"%s/gen-cpp-typelib"} % out_path)) ;
  string dest = str(boost::format{"%s/gen-cpp-typelib/%s_typelib.cpp"} %
		    out_path % name) ;

  if (!exists(destdir)) create_directory(destdir) ;
  ofstream out ;
  out.open(dest, ios::out | ios::trunc) ;
  if (out.fail()) {
    std::cerr << "Cannot open file " << dest << " for write" << std::endl ;
    exit(-1) ;
  }

  out << str(boost::format{
R"FOO(
#include "NiceJSON.h"

namespace %s {
struct StaticInitializer_%s {
  StaticInitializer_%s() : json_(
R"WIREJSON()FOO"} % ns % name % name) ;

  out << apache::thrift::ThriftJSONString(input) ;
  out << R"FOO()WIREJSON") {
}

apache::thrift::nicejson::NiceJSON json_ ;
} json_ ;

}
)FOO";
}
