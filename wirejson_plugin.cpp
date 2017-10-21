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

#include <boost/smart_ptr.hpp>
#include "thrift/plugin/plugin.h"
#include "gen-cpp/plugin_types.h"
#include "thrift/generate/t_generator.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>
#include "thrift/transport/TFDTransport.h"

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
    return -1;
  }

  std::string serialized ;
  {
    serialized = apache::thrift::ThriftJSONString(input) ;
    std::cout << serialized << std::endl ;
  }
}
