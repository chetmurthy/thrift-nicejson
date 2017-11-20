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

#ifndef _THRIFT_PROTOCOL_TNICEJSONPROTOCOL_H_
#define _THRIFT_PROTOCOL_TNICEJSONPROTOCOL_H_ 1

#include <thrift/protocol/TVirtualProtocol.h>
#include <thrift/transport/TBufferTransports.h>

#include "json.hpp"

using nlohmann::json;
using namespace apache::thrift::transport;

class transport_input_adapter : public nlohmann::detail::input_adapter_protocol {
public:
  transport_input_adapter(boost::shared_ptr<TTransport> trans) : xprt_(trans) {
  }

  transport_input_adapter(const std::string ser) {
    boost::shared_ptr<TMemoryBuffer> mem(new TMemoryBuffer((uint8_t*)ser.data(),
							   static_cast<uint32_t>(ser.length()))) ;
    xprt_ = mem ;
  }

  int get_character() override {
    uint8_t c ;
    if (1 != xprt_->read(&c, 1)) {
      return std::char_traits<char>::eof() ;
    } else {
      buf.push_back(static_cast<char>(c)) ;
      return c ;
    }
  }

  std::string read(std::size_t offset, std::size_t length) override {
    if (offset > buf.size() || offset + length > buf.size())
      throw nlohmann::detail::parse_error::create(0, buf.size(), "read() called with invalid arguments") ;

    return std::string(&buf[offset], length) ;
  }

  std::vector<char> buf ;
  boost::shared_ptr<TTransport> xprt_ ;
} ;

json parse_via_transport(const std::string& s) {
  using nlohmann::detail::input_adapter ;
  using nlohmann::detail::input_adapter_protocol ;
  json j = nlohmann::json::parse(input_adapter(std::shared_ptr<input_adapter_protocol>(new transport_input_adapter(s)))) ;
  return j ;
}

namespace apache {
namespace thrift {
namespace protocol {

class TNiceJSONProtocol : public TVirtualProtocol<TNiceJSONProtocol> {
public:
  TNiceJSONProtocol(boost::shared_ptr<TTransport> ptrans);

  ~TNiceJSONProtocol();

public:
  /**
   * Writing functions.
   */

  uint32_t writeMessageBegin(const std::string& name,
                             const TMessageType messageType,
                             const int32_t seqid);

  uint32_t writeMessageEnd();

  uint32_t writeStructBegin(const char* name);

  uint32_t writeStructEnd();

  uint32_t writeFieldBegin(const char* name, const TType fieldType, const int16_t fieldId);

  uint32_t writeFieldEnd();

  uint32_t writeFieldStop();

  uint32_t writeMapBegin(const TType keyType, const TType valType, const uint32_t size);

  uint32_t writeMapEnd();

  uint32_t writeListBegin(const TType elemType, const uint32_t size);

  uint32_t writeListEnd();

  uint32_t writeSetBegin(const TType elemType, const uint32_t size);

  uint32_t writeSetEnd();

  uint32_t writeBool(const bool value);

  uint32_t writeByte(const int8_t byte);

  uint32_t writeI16(const int16_t i16);

  uint32_t writeI32(const int32_t i32);

  uint32_t writeI64(const int64_t i64);

  uint32_t writeDouble(const double dub);

  uint32_t writeString(const std::string& str);

  uint32_t writeBinary(const std::string& str);

  /**
   * Reading functions
   */

  uint32_t readMessageBegin(std::string& name, TMessageType& messageType, int32_t& seqid);

  uint32_t readMessageEnd();

  uint32_t readStructBegin(std::string& name);

  uint32_t readStructEnd();

  uint32_t readFieldBegin(std::string& name, TType& fieldType, int16_t& fieldId);

  uint32_t readFieldEnd();

  uint32_t readMapBegin(TType& keyType, TType& valType, uint32_t& size);

  uint32_t readMapEnd();

  uint32_t readListBegin(TType& elemType, uint32_t& size);

  uint32_t readListEnd();

  uint32_t readSetBegin(TType& elemType, uint32_t& size);

  uint32_t readSetEnd();

  uint32_t readBool(bool& value);

  // Provide the default readBool() implementation for std::vector<bool>
  using TVirtualProtocol<TNiceJSONProtocol>::readBool;

  uint32_t readByte(int8_t& byte);

  uint32_t readI16(int16_t& i16);

  uint32_t readI32(int32_t& i32);

  uint32_t readI64(int64_t& i64);

  uint32_t readDouble(double& dub);

  uint32_t readString(std::string& str);

  uint32_t readBinary(std::string& str);

private:
  TTransport* trans_;
};

/**
 * Constructs input and output protocol objects given transports.
 */
class TNiceJSONProtocolFactory : public TProtocolFactory {
public:
  TNiceJSONProtocolFactory() {}

  virtual ~TNiceJSONProtocolFactory() {}

  boost::shared_ptr<TProtocol> getProtocol(boost::shared_ptr<TTransport> trans) {
    return boost::shared_ptr<TProtocol>(new TNiceJSONProtocol(trans));
  }
};
}
}
} // apache::thrift::protocol

#endif // #define _THRIFT_PROTOCOL_TNICEJSONPROTOCOL_H_ 1
