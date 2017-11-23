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

#include "TNiceJSONProtocol.h"

#include <boost/lexical_cast.hpp>
#include <boost/locale.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include <cmath>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>

#include <thrift/protocol/TBase64Utils.h>
#include <thrift/transport/TTransportException.h>

using namespace apache::thrift::transport;

json parse_via_transport(const std::string& s) {
  using nlohmann::detail::input_adapter ;
  using nlohmann::detail::input_adapter_protocol ;
  auto i = input_adapter(std::shared_ptr<input_adapter_protocol>(new convenient_transport_input_adapter(s))) ;
  json j ;
  nlohmann::json::parser(i, nullptr, true).parse(false, j);
  return j ;
}

namespace apache {
namespace thrift {
namespace protocol {


  TNiceJSONProtocol::TNiceJSONProtocol(const std::string typelib, const std::string service,
				       boost::shared_ptr<TTransport> ptrans)
  : TVirtualProtocol<TNiceJSONProtocol>(ptrans),
    trans_(ptrans.get()),
    mode_(NONE),
    message_name_("") ,
    message_type_(T_CALL) ,
    message_seqid_(-1l) ,
    message_buffer_(nullptr),
    message_proto_(nullptr),
    service_(service),
    typelib_(*NiceJSON::lookup_typelib(typelib))
  {
  }

TNiceJSONProtocol::~TNiceJSONProtocol() {
}

inline void TNiceJSONProtocol::must_be_writing() {
  if (mode_ == WRITING_MESSAGE) return ;
  throw TProtocolException("TNiceJSONProtocol: calling sequence mismatch") ;
}
inline void TNiceJSONProtocol::must_be_reading() {
  if (mode_ == READING_MESSAGE) return ;
  throw TProtocolException("TNiceJSONProtocol: calling sequence mismatch") ;
}
inline void TNiceJSONProtocol::must_be_none() {
  if (mode_ == NONE) return ;
  throw TProtocolException("TNiceJSONProtocol: calling sequence mismatch") ;
}

uint32_t TNiceJSONProtocol::writeMessageBegin(const std::string& name,
                                          const TMessageType messageType,
                                          const int32_t seqid) {
  must_be_none() ;
  message_name_ = name ;
  message_type_ = messageType ;
  message_seqid_ = seqid ;

  boost::shared_ptr<TMemoryBuffer> newbuf(new TMemoryBuffer()) ;
  boost::shared_ptr<TBinaryProtocol> newproto(new TBinaryProtocol(newbuf)) ;

  message_buffer_.swap(newbuf) ;
  message_proto_.swap(newproto) ;

  mode_ = WRITING_MESSAGE ;
  return 0l ;
}

std::string struct_name(const NiceJSON& typelib, const std::string service, const std::string name, TMessageType messageType) {
  switch (messageType) {
  case T_CALL: {
    return typelib.service_struct_names(service, name).first ;
    break ;
  }
  case T_REPLY: {
    return typelib.service_struct_names(service, name).second ;
    break ;
  }
  case T_EXCEPTION: {
    assert(false) ;
  }
  default:
    assert(false) ;
  }
}

std::string json_message_type(TMessageType messageType) {
  switch (messageType) {
  case T_CALL: {
   return "call" ;
    break ;
  }
  case T_REPLY: {
    return "reply" ;
    break ;
  }
  case T_EXCEPTION: {
    return "exception" ;
    break ;
  }
  default:
    assert(false) ;
  }
}

using ::apache::thrift::TApplicationException ;

TApplicationException::TApplicationExceptionType string_to_TApplicationExceptionType(const std::string& s) {
  if ("UNKNOWN" == s) return TApplicationException::UNKNOWN ;
  if ("UNKNOWN_METHOD" == s) return TApplicationException::UNKNOWN_METHOD ;
  if ("INVALID_MESSAGE_TYPE" == s) return TApplicationException::INVALID_MESSAGE_TYPE ;
  if ("WRONG_METHOD_NAME" == s) return TApplicationException::WRONG_METHOD_NAME ;
  if ("BAD_SEQUENCE_ID" == s) return TApplicationException::BAD_SEQUENCE_ID ;
  if ("MISSING_RESULT" == s) return TApplicationException::MISSING_RESULT ;
  if ("INTERNAL_ERROR" == s) return TApplicationException::INTERNAL_ERROR ;
  if ("PROTOCOL_ERROR" == s) return TApplicationException::PROTOCOL_ERROR ;
  if ("INVALID_TRANSFORM" == s) return TApplicationException::INVALID_TRANSFORM ;
  if ("INVALID_PROTOCOL" == s) return TApplicationException::INVALID_PROTOCOL ;
  if ("UNSUPPORTED_CLIENT_TYPE" == s) return TApplicationException::UNSUPPORTED_CLIENT_TYPE ;
  return TApplicationException::UNKNOWN ;
}

std::string TApplicationExceptionType_to_string(TApplicationException::TApplicationExceptionType t) {
  switch (t) {
  case TApplicationException::UNKNOWN: return "UNKNOWN" ;
  case TApplicationException::UNKNOWN_METHOD: return "UNKNOWN_METHOD" ;
  case TApplicationException::INVALID_MESSAGE_TYPE: return "INVALID_MESSAGE_TYPE" ;
  case TApplicationException::WRONG_METHOD_NAME: return "WRONG_METHOD_NAME" ;
  case TApplicationException::BAD_SEQUENCE_ID: return "BAD_SEQUENCE_ID" ;
  case TApplicationException::MISSING_RESULT: return "MISSING_RESULT" ;
  case TApplicationException::INTERNAL_ERROR: return "INTERNAL_ERROR" ;
  case TApplicationException::PROTOCOL_ERROR: return "PROTOCOL_ERROR" ;
  case TApplicationException::INVALID_TRANSFORM: return "INVALID_TRANSFORM" ;
  case TApplicationException::INVALID_PROTOCOL: return "INVALID_PROTOCOL" ;
  case TApplicationException::UNSUPPORTED_CLIENT_TYPE: return "UNSUPPORTED_CLIENT_TYPE" ;
  default:
    return "UNKNOWN" ;
  }
}

uint32_t TNiceJSONProtocol::writeMessageEnd() {
  must_be_writing() ;
  mode_ = BROKEN ; // so if we fail, the transport is marked broken ;

  std::string json_msgType = json_message_type(message_type_) ;
  
  json j;
  if (message_type_ == T_EXCEPTION) {
    ::apache::thrift::TApplicationException x ;
    x.read(message_proto_.get()) ;
    j = { { "message", x.what() }, { "type" , TApplicationExceptionType_to_string(x.getType()) } } ;
  } else {
    std::string structname = struct_name(typelib_, service_, message_name_, message_type_) ;
    j = typelib_.marshal_from_binary(structname, message_buffer_) ;
  }
  json msg ;
  msg["body"] = j ;
  msg["name"] = message_name_ ;
  msg["type"] = json_msgType ;
  msg["seqid"] = message_seqid_ ;
  std::string js = msg.dump() ;
  trans_->write(reinterpret_cast<const uint8_t*>(js.data()), js.length()) ;

  mode_ = NONE ;
  return 0l ;
}

uint32_t TNiceJSONProtocol::writeStructBegin(const char* name) {
  must_be_writing() ;
  return message_proto_->writeStructBegin(name) ;
}

uint32_t TNiceJSONProtocol::writeStructEnd() {
  must_be_writing() ;
  return message_proto_->writeStructEnd() ;
}

uint32_t TNiceJSONProtocol::writeFieldBegin(const char* name,
                                        const TType fieldType,
                                        const int16_t fieldId) {
  must_be_writing() ;
  return message_proto_->writeFieldBegin(name, fieldType, fieldId) ;
}

uint32_t TNiceJSONProtocol::writeFieldEnd() {
  must_be_writing() ;
  return message_proto_->writeFieldEnd() ;
}

uint32_t TNiceJSONProtocol::writeFieldStop() {
  must_be_writing() ;
  return message_proto_->writeFieldStop() ;
}

uint32_t TNiceJSONProtocol::writeMapBegin(const TType keyType,
                                      const TType valType,
                                      const uint32_t size) {
  must_be_writing() ;
  return message_proto_->writeMapBegin(keyType, valType, size) ;
}

uint32_t TNiceJSONProtocol::writeMapEnd() {
  must_be_writing() ;
  return message_proto_->writeMapEnd() ;
}

uint32_t TNiceJSONProtocol::writeListBegin(const TType elemType, const uint32_t size) {
  must_be_writing() ;
  return message_proto_->writeListBegin(elemType, size) ;
}

uint32_t TNiceJSONProtocol::writeListEnd() {
  must_be_writing() ;
  return message_proto_->writeListEnd() ;
}

uint32_t TNiceJSONProtocol::writeSetBegin(const TType elemType, const uint32_t size) {
  must_be_writing() ;
  return message_proto_->writeSetBegin(elemType, size) ;
}

uint32_t TNiceJSONProtocol::writeSetEnd() {
  must_be_writing() ;
  return message_proto_->writeSetEnd() ;
}

uint32_t TNiceJSONProtocol::writeBool(const bool value) {
  must_be_writing() ;
  return message_proto_->writeBool(value) ;
}

uint32_t TNiceJSONProtocol::writeByte(const int8_t byte) {
  must_be_writing() ;
  return message_proto_->writeByte(byte) ;
}

uint32_t TNiceJSONProtocol::writeI16(const int16_t i16) {
  must_be_writing() ;
  return message_proto_->writeI16(i16) ;
}

uint32_t TNiceJSONProtocol::writeI32(const int32_t i32) {
  must_be_writing() ;
  return message_proto_->writeI32(i32) ;
}

uint32_t TNiceJSONProtocol::writeI64(const int64_t i64) {
  must_be_writing() ;
  return message_proto_->writeI64(i64) ;
}

uint32_t TNiceJSONProtocol::writeDouble(const double dub) {
  must_be_writing() ;
  return message_proto_->writeDouble(dub) ;
}

uint32_t TNiceJSONProtocol::writeString(const std::string& str) {
  must_be_writing() ;
  return message_proto_->writeString(str) ;
}

uint32_t TNiceJSONProtocol::writeBinary(const std::string& str) {
  must_be_writing() ;
  return message_proto_->writeBinary(str) ;
}

/**
 * Reading functions
 */


uint32_t TNiceJSONProtocol::readMessageBegin(std::string& name,
                                         TMessageType& messageType,
                                         int32_t& seqid) {
  must_be_none() ;
  using nlohmann::detail::input_adapter ;
  using nlohmann::detail::input_adapter_protocol ;
  json j ;
  try {
    auto ia = input_adapter(std::shared_ptr<input_adapter_protocol>(new transport_input_adapter(trans_))) ;
    nlohmann::json::parser(ia, nullptr, true).parse(false, j);
  } catch (nlohmann::detail::parse_error e) {
    throw TProtocolException(str(boost::format{"TNiceJSONProtocol: parse error: %s"} % e.what())) ;
  }
  {
    if (j.count("type") == 0)
      throw TProtocolException("TNiceJSONProtocol: bad JSON, missing type element") ;
    json j_messageType = j["type"] ;
    if (!j_messageType.is_string()) 
      throw TProtocolException("TNiceJSONProtocol: bad JSON, type element must be string") ;
    std::string s_messageType = j_messageType.get<std::string>() ;
    if (s_messageType == "call") message_type_ = T_CALL ;
    else if (s_messageType == "reply") message_type_ = T_REPLY ;
    else if (s_messageType == "exception") message_type_ = T_EXCEPTION ;
    else
      throw TProtocolException("TNiceJSONProtocol: bad JSON, type element must be either call or reply") ;
  }

  {
    if (j.count("name") == 0)
      throw TProtocolException("TNiceJSONProtocol: bad JSON, missing name element") ;
    json j_name = j["name"] ;
    if (!j_name.is_string())
      throw TProtocolException("TNiceJSONProtocol: bad JSON, name element must be string") ;
    message_name_ = j_name.get<std::string>() ;
  }

  {
    if (j.count("seqid") == 0)
      throw TProtocolException("TNiceJSONProtocol: bad JSON, missing seqid element") ;
    json j_seqid = j["seqid"] ;
    if (!j_seqid.is_number())
      throw TProtocolException("TNiceJSONProtocol: bad JSON, seqid element must be number") ;
    message_seqid_ = j_seqid.get<uint32_t>() ;
  }

  if (j.count("body") == 0)
    throw TProtocolException("TNiceJSONProtocol: bad JSON, missing body element") ;
  
  boost::shared_ptr<TMemoryBuffer> newbuf(new TMemoryBuffer()) ;
  boost::shared_ptr<TBinaryProtocol> newproto(new TBinaryProtocol(newbuf)) ;

  message_buffer_.swap(newbuf) ;
  message_proto_.swap(newproto) ;

  if (message_type_ != T_EXCEPTION) {
    std::string structname = struct_name(typelib_, service_, message_name_, message_type_) ;
    typelib_.demarshal_to_binary(structname, j["body"], message_proto_) ;
  } else {
    json body = j["body"] ;
    if (body.count("message") == 0 || body.count("type") == 0) {
      ::apache::thrift::TApplicationException x(::apache::thrift::TApplicationException::UNKNOWN, "malformed exception " + body.dump()) ;
      x.write(message_proto_.get()) ;
    } else {
      auto ty = string_to_TApplicationExceptionType(body["type"].get<std::string>()) ;
      auto message = body["message"].get<std::string>() ;
	::apache::thrift::TApplicationException x(ty, message) ;
	x.write(message_proto_.get()) ;
    }
  }

  mode_ = READING_MESSAGE ;

  name = message_name_ ;
  messageType = message_type_ ;
  seqid = message_seqid_ ;
  return 0l ;
}

uint32_t TNiceJSONProtocol::readMessageEnd() {
  must_be_reading() ;

  boost::shared_ptr<TMemoryBuffer> newbuf ;
  boost::shared_ptr<TBinaryProtocol> newproto ;

  message_buffer_.swap(newbuf) ;
  message_proto_.swap(newproto) ;

  mode_ = NONE ;
  return 0l ;
}

uint32_t TNiceJSONProtocol::readStructBegin(std::string& name) {
  must_be_reading() ;
  (void)name;
  return message_proto_->readStructBegin(name) ;
}

uint32_t TNiceJSONProtocol::readStructEnd() {
  must_be_reading() ;
  return message_proto_->readStructEnd() ;
}

uint32_t TNiceJSONProtocol::readFieldBegin(std::string& name, TType& fieldType, int16_t& fieldId) {
  must_be_reading() ;
  (void)name;
  return message_proto_->readFieldBegin(name, fieldType, fieldId) ;
}

uint32_t TNiceJSONProtocol::readFieldEnd() {
  must_be_reading() ;
  return message_proto_->readFieldEnd() ;
}

uint32_t TNiceJSONProtocol::readMapBegin(TType& keyType, TType& valType, uint32_t& size) {
  must_be_reading() ;
  return message_proto_->readMapBegin(keyType, valType, size) ;
}

uint32_t TNiceJSONProtocol::readMapEnd() {
  must_be_reading() ;
  return message_proto_->readMapEnd() ;
}

uint32_t TNiceJSONProtocol::readListBegin(TType& elemType, uint32_t& size) {
  must_be_reading() ;
  return message_proto_->readListBegin(elemType, size) ;
}

uint32_t TNiceJSONProtocol::readListEnd() {
  must_be_reading() ;
  return message_proto_->readListEnd() ;
}

uint32_t TNiceJSONProtocol::readSetBegin(TType& elemType, uint32_t& size) {
  must_be_reading() ;
  return message_proto_->readSetBegin(elemType, size) ;
}

uint32_t TNiceJSONProtocol::readSetEnd() {
  must_be_reading() ;
  return message_proto_->readSetEnd() ;
}

uint32_t TNiceJSONProtocol::readBool(bool& value) {
  must_be_reading() ;
  return message_proto_->readBool(value) ;
}

// readByte() must be handled properly because boost::lexical cast sees int8_t
// as a text type instead of an integer type
uint32_t TNiceJSONProtocol::readByte(int8_t& byte) {
  must_be_reading() ;
  return message_proto_->readByte(byte) ;
}

uint32_t TNiceJSONProtocol::readI16(int16_t& i16) {
  must_be_reading() ;
  return message_proto_->readI16(i16) ;
}

uint32_t TNiceJSONProtocol::readI32(int32_t& i32) {
  must_be_reading() ;
  return message_proto_->readI32(i32) ;
}

uint32_t TNiceJSONProtocol::readI64(int64_t& i64) {
  must_be_reading() ;
  return message_proto_->readI64(i64) ;
}

uint32_t TNiceJSONProtocol::readDouble(double& dub) {
  must_be_reading() ;
  return message_proto_->readDouble(dub) ;
}

uint32_t TNiceJSONProtocol::readString(std::string& str) {
  must_be_reading() ;
  return message_proto_->readString(str) ;
}

uint32_t TNiceJSONProtocol::readBinary(std::string& str) {
  must_be_reading() ;
  return message_proto_->readBinary(str) ;
}
}
}
} // apache::thrift::protocol
