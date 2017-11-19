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
#include <iterator>
#include <map>
#include <regex>
#include <string>

#include "boost/filesystem.hpp"
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include "thrift/transport/TFDTransport.h"

#include "NiceJSON.h"
#include "ns_utils.h"
#include "thrift/plugin/plugin.h"
#include "gen-cpp/plugin_types.h"

using namespace std ;
using namespace boost::filesystem;
using apache::thrift::protocol::TBinaryProtocol;
using apache::thrift::transport::TFDTransport;
using apache::thrift::transport::TFramedTransport;

void gen_typelib(const apache::thrift::plugin::GeneratorInput& input) {

  string cpp_ns ;
  {
    map<string, string>::const_iterator ii;
    if (input.program.namespaces.end() != (ii = input.program.namespaces.find("cpp"))) {
      cpp_ns = ii->second ;
    }
    else {
      std::cerr << "(required) cpp namespace NOT declared" << std::endl ;
      exit(-1) ;
    }
  }

  string typelib_ns ;
  {
    map<string, string>::const_iterator ii;
    if (input.program.namespaces.end() != (ii = input.program.namespaces.find("typelib"))) {
      typelib_ns = ii->second ;
    }
    else typelib_ns = cpp_ns ;
  }

  string out_path = input.program.out_path ;
  string name = input.program.name ;
  path destdir(str(boost::format{"%s/gen-typelib"} % out_path)) ;
  string dest = str(boost::format{"%s/gen-typelib/%s.%s.typelib"} %
		       out_path % typelib_ns % name) ;
  string binary_dest = str(boost::format{"%s/gen-typelib/%s.%s.binary_typelib"} %
		       out_path % typelib_ns % name) ;

  if (!exists(destdir)) create_directory(destdir) ;
  {
    ofstream out ;
    out.open(dest, ios::out | ios::trunc) ;
    if (out.fail()) {
      std::cerr << "Cannot open file " << dest << " for write" << std::endl ;
      exit(-1) ;
    }

    out << apache::thrift::ThriftJSONString(input) ;
  }
  {
    ofstream out ;
    out.open(binary_dest, ios::out | ios::trunc) ;
    if (out.fail()) {
      std::cerr << "Cannot open file " << binary_dest << " for write" << std::endl ;
      exit(-1) ;
    }

    out << ThriftBinaryString(input) ;
  }
}

void generate1(const std::string& full_structname, const std::string& name,
	       std::ostream *cppout, std::ostream* hout) {
      *cppout << str(boost::format{R"FOO(
  void demarshal(const json& j, %s *out) {
    json_.json_.demarshal("%s", j, out) ;
  }

  json marshal(const %s& in) {
    return json_.json_.marshal("%s", in) ;
  }
)FOO"} % full_structname % name % full_structname % name);

      *hout << str(boost::format{R"FOO(
  void demarshal(const json& j, %s *out) ;

  json marshal(const %s& in) ;
)FOO"} % full_structname % full_structname);
}

void gen_cpp_typelib(const apache::thrift::plugin::GeneratorInput& input) {

  apache::thrift::nicejson::NiceJSON nj(input) ;

  string cpp_ns ;
  {
    map<string, string>::const_iterator ii;
    if (input.program.namespaces.end() != (ii = input.program.namespaces.find("cpp"))) {
      cpp_ns = ii->second ;
    }
    else {
      std::cerr << "(required) cpp namespace NOT declared" << std::endl ;
      exit(-1) ;
    }
  }

  string typelib_ns ;
  {
    map<string, string>::const_iterator ii;
    if (input.program.namespaces.end() != (ii = input.program.namespaces.find("typelib"))) {
      typelib_ns = ii->second ;
    }
    else typelib_ns = cpp_ns ;
  }

  string out_path = input.program.out_path ;
  string name = input.program.name ;
  path cppdestdir(str(boost::format{"%s/gen-cpp-typelib"} % out_path)) ;
  string cppdest = str(boost::format{"%s/gen-cpp-typelib/%s_typelib.cpp"} %
		       out_path % name) ;
  string hdest = str(boost::format{"%s/gen-cpp-typelib/%s_typelib.h"} %
		       out_path % name) ;

  if (!exists(cppdestdir)) create_directory(cppdestdir) ;
  ofstream cppout ;
  cppout.open(cppdest, ios::out | ios::trunc) ;
  if (cppout.fail()) {
    std::cerr << "Cannot open file " << cppdest << " for write" << std::endl ;
    exit(-1) ;
  }
  ofstream hout ;
  hout.open(hdest, ios::out | ios::trunc) ;
  if (hout.fail()) {
    std::cerr << "Cannot open file " << hdest << " for write" << std::endl ;
    exit(-1) ;
  }

  cppout << str(boost::format{
R"FOO(
#include "gen-cpp-typelib/%s_typelib.h"
%s
struct StaticInitializer_%s {
  StaticInitializer_%s() : json_(*apache::thrift::nicejson::NiceJSON::install_typelib("%s", "%s",
R"WIREJSON()FOO"} % name % ns_open(cpp_ns) % name % name % typelib_ns % name) ;

  cppout << apache::thrift::ThriftJSONString(input) ;
  cppout << R"FOO()WIREJSON")) {
}

const apache::thrift::nicejson::NiceJSON& json_ ;
} json_ ;

)FOO" ;

  hout << R"FOO(#include "NiceJSON.h")FOO" << std::endl ;
  hout << str(boost::format{"#include \"gen-cpp/%s_types.h\""} % name) << std::endl ;

  using apache::thrift::plugin::t_type_id ;
  using apache::thrift::plugin::t_function ;

  auto allservices = input.type_registry.services ;
  auto alltypes = input.type_registry.types ;

  for (auto ii = allservices.begin(); ii != allservices.end() ; ++ii) {
    const std::string& servicename = ii->second.metadata.name ;
    hout << str(boost::format{"#include \"gen-cpp/%s.h\""} % servicename) << std::endl ;
  }

  hout << str(boost::format{
R"FOO(
#ifndef %s_typelib_INCLUDED
#define %s_typelib_INCLUDED
%s
)FOO"} % name % name % ns_open(cpp_ns)) ;

  for(auto ii = nj.structs_by_name.begin() ; ii != nj.structs_by_name.end() ; ++ii) {
    generate1(ii->first, ii->first, &cppout, &hout) ;
  }


  cppout << str(boost::format{R"FOO(%s)FOO"} % ns_close(cpp_ns)) ;
  hout << str(boost::format{R"FOO(
%s
#endif
)FOO"} % ns_close(cpp_ns)) ;

}

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

  for (auto ii = input.parsed_options.begin() ; ii != input.parsed_options.end() ; ++ii) {
    if (ii->first == "cpp") {
      gen_cpp_typelib(input) ;
    }
    else if (ii->first == "typelib") {
      gen_typelib(input) ;
    }
    else {
      cerr << boost::format{"Unrecognized typelib target %s\n"} % ii->first ;
    }
  }
}
