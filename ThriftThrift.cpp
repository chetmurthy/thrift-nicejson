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

#include <thrift/protocol/TDebugProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>

#include "../gen-cpp/thrift_thrift_types.h"

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

#include "gen-cpp/thrift_thrift_types.h"

using namespace thrift_thrift;

int main(int ac, char **av) {
  {
    Foo a ;
    a.__set_a(1) ;
    a.__set_b("ugh") ;
    a.__set_c(Bar()) ;
    a.c.__set_a(2) ;
    a.c.__set_b("argh");
    if (true) {
      a.d.push_back(a.c) ;
      a.d.push_back(a.c) ;
      a.e["foo"] = a.c ;
      a.e["bar"] = a.c ;
      a.g.insert(32) ;
      a.j = map<string, set<int32_t> >{
	{"foo", {1,2,3}},
	{"bar", {4,5,6}},
      } ;
    }
    std::string serialized ;
    {
      serialized = apache::thrift::ThriftJSONString(a) ;
      cout << serialized << std::endl ;
    }

    Foo a2 ;
    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TJSONProtocol> p(new TJSONProtocol(buf)); 
    
      buf->resetBuffer((uint8_t*)serialized.data(), static_cast<uint32_t>(serialized.length()));
      a2.read(p.get());

    }
    assert(a == a2) ;

    {
      shared_ptr<TMemoryBuffer> buf(new TMemoryBuffer());
      shared_ptr<TDebugProtocol> p(new TDebugProtocol(buf)); 
    
      a.write(p.get()) ;
      cout << buf->getBufferAsString() << std::endl ;
    }

  }
}
