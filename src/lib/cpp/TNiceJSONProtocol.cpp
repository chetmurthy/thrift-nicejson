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

namespace apache {
namespace thrift {
namespace protocol {


TNiceJSONProtocol::TNiceJSONProtocol(boost::shared_ptr<TTransport> ptrans)
  : TVirtualProtocol<TNiceJSONProtocol>(ptrans),
  trans_(ptrans.get()) {
}

TNiceJSONProtocol::~TNiceJSONProtocol() {
}

uint32_t TNiceJSONProtocol::writeMessageBegin(const std::string& name,
                                          const TMessageType messageType,
                                          const int32_t seqid) {
}

uint32_t TNiceJSONProtocol::writeMessageEnd() {
}

uint32_t TNiceJSONProtocol::writeStructBegin(const char* name) {
  (void)name;
}

uint32_t TNiceJSONProtocol::writeStructEnd() {
}

uint32_t TNiceJSONProtocol::writeFieldBegin(const char* name,
                                        const TType fieldType,
                                        const int16_t fieldId) {
  (void)name;
}

uint32_t TNiceJSONProtocol::writeFieldEnd() {
}

uint32_t TNiceJSONProtocol::writeFieldStop() {
}

uint32_t TNiceJSONProtocol::writeMapBegin(const TType keyType,
                                      const TType valType,
                                      const uint32_t size) {
}

uint32_t TNiceJSONProtocol::writeMapEnd() {
}

uint32_t TNiceJSONProtocol::writeListBegin(const TType elemType, const uint32_t size) {
}

uint32_t TNiceJSONProtocol::writeListEnd() {
}

uint32_t TNiceJSONProtocol::writeSetBegin(const TType elemType, const uint32_t size) {
}

uint32_t TNiceJSONProtocol::writeSetEnd() {
}

uint32_t TNiceJSONProtocol::writeBool(const bool value) {
}

uint32_t TNiceJSONProtocol::writeByte(const int8_t byte) {
}

uint32_t TNiceJSONProtocol::writeI16(const int16_t i16) {
}

uint32_t TNiceJSONProtocol::writeI32(const int32_t i32) {
}

uint32_t TNiceJSONProtocol::writeI64(const int64_t i64) {
}

uint32_t TNiceJSONProtocol::writeDouble(const double dub) {
}

uint32_t TNiceJSONProtocol::writeString(const std::string& str) {
}

uint32_t TNiceJSONProtocol::writeBinary(const std::string& str) {
}

/**
 * Reading functions
 */


uint32_t TNiceJSONProtocol::readMessageBegin(std::string& name,
                                         TMessageType& messageType,
                                         int32_t& seqid) {
}

uint32_t TNiceJSONProtocol::readMessageEnd() {
}

uint32_t TNiceJSONProtocol::readStructBegin(std::string& name) {
  (void)name;
}

uint32_t TNiceJSONProtocol::readStructEnd() {
}

uint32_t TNiceJSONProtocol::readFieldBegin(std::string& name, TType& fieldType, int16_t& fieldId) {
  (void)name;
}

uint32_t TNiceJSONProtocol::readFieldEnd() {
}

uint32_t TNiceJSONProtocol::readMapBegin(TType& keyType, TType& valType, uint32_t& size) {
}

uint32_t TNiceJSONProtocol::readMapEnd() {
}

uint32_t TNiceJSONProtocol::readListBegin(TType& elemType, uint32_t& size) {
}

uint32_t TNiceJSONProtocol::readListEnd() {
}

uint32_t TNiceJSONProtocol::readSetBegin(TType& elemType, uint32_t& size) {
}

uint32_t TNiceJSONProtocol::readSetEnd() {
}

uint32_t TNiceJSONProtocol::readBool(bool& value) {
}

// readByte() must be handled properly because boost::lexical cast sees int8_t
// as a text type instead of an integer type
uint32_t TNiceJSONProtocol::readByte(int8_t& byte) {
}

uint32_t TNiceJSONProtocol::readI16(int16_t& i16) {
}

uint32_t TNiceJSONProtocol::readI32(int32_t& i32) {
}

uint32_t TNiceJSONProtocol::readI64(int64_t& i64) {
}

uint32_t TNiceJSONProtocol::readDouble(double& dub) {
}

uint32_t TNiceJSONProtocol::readString(std::string& str) {
}

uint32_t TNiceJSONProtocol::readBinary(std::string& str) {
}
}
}
} // apache::thrift::protocol
