import json
import NiceJSON
import thrift_nicejson_binary
from thrift.Thrift import TMessageType, TException, TApplicationException
from thrift.protocol.TProtocol import TType, TProtocolBase, TProtocolException
from thrift.protocol.TBinaryProtocol import TBinaryProtocol
from thrift.transport import TTransport
from thrift.transport.TTransport import TBufferedTransport, TMemoryBuffer

MODE_NONE = 0
WRITING_MESSAGE = 1
READING_MESSAGE = 2
BROKEN = 3

def struct_name(typelib, service, name, messageType):
    if messageType == TMessageType.CALL or messageType == TMessageType.ONEWAY:
        return NiceJSON.service_struct_name_args(typelib, service, str(name))
    elif messageType == TMessageType.REPLY: return NiceJSON.service_struct_name_result(typelib, service, str(name))
    else: raise TException("struct_name: unexpected type %d" % messageType)
        
def json_message_type(messageType):
    if messageType == TMessageType.CALL: return "call"
    elif messageType == TMessageType.REPLY: return "reply"
    elif messageType == TMessageType.EXCEPTION: return "exception"
    elif messageType == TMessageType.ONEWAY: return "oneway"
    else: raise TException("json_message_type: unexpected type %d" % messageType)

def string_to_TApplicationExceptionType(s):
    if s == "UNKNOWN": return TApplicationException.UNKNOWN
    if s == "UNKNOWN_METHOD": return TApplicationException.UNKNOWN_METHOD
    if s == "INVALID_MESSAGE_TYPE": return TApplicationException.INVALID_MESSAGE_TYPE
    if s == "WRONG_METHOD_NAME": return TApplicationException.WRONG_METHOD_NAME
    if s == "BAD_SEQUENCE_ID": return TApplicationException.BAD_SEQUENCE_ID
    if s == "MISSING_RESULT": return TApplicationException.MISSING_RESULT
    if s == "INTERNAL_ERROR": return TApplicationException.INTERNAL_ERROR
    if s == "PROTOCOL_ERROR": return TApplicationException.PROTOCOL_ERROR
    if s == "INVALID_TRANSFORM": return TApplicationException.INVALID_TRANSFORM
    if s == "INVALID_PROTOCOL": return TApplicationException.INVALID_PROTOCOL
    if s == "UNSUPPORTED_CLIENT_TYPE": return TApplicationException.UNSUPPORTED_CLIENT_TYPE
    raise TException("string_to_TApplicationExceptionType: unexpected string %s" % s)

def TApplicationExceptionType_to_string(ty):
    if ty == TApplicationException.UNKNOWN: return "UNKNOWN"
    if ty == TApplicationException.UNKNOWN_METHOD: return "UNKNOWN_METHOD"
    if ty == TApplicationException.INVALID_MESSAGE_TYPE: return "INVALID_MESSAGE_TYPE"
    if ty == TApplicationException.WRONG_METHOD_NAME: return "WRONG_METHOD_NAME"
    if ty == TApplicationException.BAD_SEQUENCE_ID: return "BAD_SEQUENCE_ID"
    if ty == TApplicationException.MISSING_RESULT: return "MISSING_RESULT"
    if ty == TApplicationException.INTERNAL_ERROR: return "INTERNAL_ERROR"
    if ty == TApplicationException.PROTOCOL_ERROR: return "PROTOCOL_ERROR"
    if ty == TApplicationException.INVALID_TRANSFORM: return "INVALID_TRANSFORM"
    if ty == TApplicationException.INVALID_PROTOCOL: return "INVALID_PROTOCOL"
    if ty == TApplicationException.UNSUPPORTED_CLIENT_TYPE: return "UNSUPPORTED_CLIENT_TYPE"
    raise TException("TApplicationExceptionType_to_string: unexpected type %d" % ty)

def readjson(trans):
    while True:
        rbuf = trans.cstringio_buf
        pos = rbuf.tell()
        all = rbuf.getvalue()
        try:
            (j, nxt) = json.JSONDecoder().raw_decode(all, pos)
            rbuf.seek(nxt)
            return j
        except ValueError:
            unread = all[pos:]
            trans.cstringio_refill(unread, 1 + len(unread))

class TNiceJSONProtocol(TProtocolBase):
    """Nice JSON implementation of the Thrift protocol driver."""

    def must_be_none(self):
        if not(self.mode_ == MODE_NONE):
            raise TProtocolException("TNiceJSONProtocol: mode mismatch (was not NONE)")

    def must_be_writing(self):
        if not(self.mode_ == WRITING_MESSAGE):
            raise TProtocolException("TNiceJSONProtocol: mode mismatch (was not WRITING)")

    def must_be_reading(self):
        if not(self.mode_ == READING_MESSAGE):
            raise TProtocolException("TNiceJSONProtocol: mode mismatch (was not READING)")

    def __init__(self, trans, typelib, service):
        TProtocolBase.__init__(self, trans)
        self.typelib_ = typelib
        self.service_ = service
        self.mode_ = MODE_NONE
        self.message_name_ = ''
        self.message_type_ = 0
        self.message_seqid_ = 0
        self.message_buffer_ = None
        self.message_proto_ = None

    def writeMessageBegin(self, name, type, seqid):
        self.must_be_none()
        self.message_name_ = name
        self.message_type_ = type
        self.message_seqid = seqid
        newbuf = TMemoryBuffer()
        transport = TBufferedTransport(newbuf)
        newproto = TBinaryProtocol(transport)
        self.message_buffer_ = newbuf
        self.message_proto_ = newproto
        self.mode_ = WRITING_MESSAGE

    def writeMessageEnd(self):
        self.must_be_writing()
        self.mode_ = BROKEN
        json_msgType = json_message_type(self.message_type_)
        j = None
        self.message_proto_.trans.flush()
        if self.message_type_ == TMessageType.EXCEPTION:
            x = TApplicationException()
            ser = self.message_buffer_.getvalue()
            newproto = TBinaryProtocol(TBufferedTransport(TMemoryBuffer(ser)))
            x.read(newproto)
            j = { 'message': str(x), 'type': TApplicationExceptionType_to_string(x.type) }
        else:
            structname = struct_name(self.typelib_, self.service_, self.message_name_, self.message_type_)
            ser = self.message_buffer_.getvalue()
            j = NiceJSON.json_from_binary(self.typelib_, structname, ser)
        msg = { 'body': j, 'name': self.message_name_, 'type': json_msgType, 'seqid': self.message_seqid_ }
        js = json.dumps(msg)
        self.trans.write(js)
        self.message_name_ = ''
        self.message_type_ = 0
        self.message_seqid_ = 0
        self.mode_ = MODE_NONE

    def writeStructBegin(self, name):
        self.must_be_writing()
        return self.message_proto_.writeStructBegin(name)

    def writeStructEnd(self):
        self.must_be_writing()
        return self.message_proto_.writeStructEnd()

    def writeFieldBegin(self, name, ttype, fid):
        self.must_be_writing()
        return self.message_proto_.writeFieldBegin(name, ttype, fid)

    def writeFieldEnd(self):
        self.must_be_writing()
        return self.message_proto_.writeFieldEnd()

    def writeFieldStop(self):
        self.must_be_writing()
        return self.message_proto_.writeFieldStop()

    def writeMapBegin(self, ktype, vtype, size):
        self.must_be_writing()
        return self.message_proto_.writeMapBegin(ktype, vtype, size)

    def writeMapEnd(self):
        self.must_be_writing()
        return self.message_proto_.writeMapEnd()

    def writeListBegin(self, etype, size):
        self.must_be_writing()
        return self.message_proto_.writeListBegin(etype, size)

    def writeListEnd(self):
        self.must_be_writing()
        return self.message_proto_.writeListEnd()

    def writeSetBegin(self, etype, size):
        self.must_be_writing()
        return self.message_proto_.writeSetBegin(etype, size)

    def writeSetEnd(self):
        self.must_be_writing()
        return self.message_proto_.writeSetEnd()

    def writeBool(self, bool_val):
        self.must_be_writing()
        return self.message_proto_.writeBool(bool_val)

    def writeByte(self, byte):
        self.must_be_writing()
        return self.message_proto_.writeByte(byte)

    def writeI16(self, i16):
        self.must_be_writing()
        return self.message_proto_.writeI16(i16)

    def writeI32(self, i32):
        self.must_be_writing()
        return self.message_proto_.writeI32(i32)

    def writeI64(self, i64):
        self.must_be_writing()
        return self.message_proto_.writeI64(i64)

    def writeDouble(self, dub):
        self.must_be_writing()
        return self.message_proto_.writeDouble(dub)

    def writeString(self, str_val):
        self.must_be_writing()
        return self.message_proto_.writeString(str_val)

    def writeBinary(self, str_val):
        self.must_be_writing()
        return self.message_proto_.writeBinary(str_val)

    def writeUtf8(self, str_val):
        self.must_be_writing()
        return self.message_proto_.writeUtf8(str_val)

    def readMessageBegin(self):
        self.must_be_none()
        j = readjson(self.trans)
        if 'type' not in j: raise TProtocolException("TNiceJSONProtocol: bad JSON, missing type element")
        j_messageType = j['type']
        if (type(j_messageType) is not unicode): raise TProtocolException("TNiceJSONProtocol: bad JSON, type element must be string")
        if j_messageType == 'call': self.message_type_ = TMessageType.CALL
        elif j_messageType == 'reply': self.message_type_ = TMessageType.REPLY
        elif j_messageType == 'exception': self.message_type_ = TMessageTyye.EXCEPTION
        elif j_messageType == 'oneway': self.message_type_ = TMessageTyye.ONEWAY
        else: raise TProtocolException("TNiceJSONProtocol: bad JSON, type element must be either call or reply")
        if 'name' not in j: raise TProtocolException("TNiceJSONProtocol: bad JSON, missing name element")
        j_name = j['name']
        if (type(j_name) is not unicode): raise TProtocolException("TNiceJSONProtocol: bad JSON, name element must be string")
        self.message_name_ = j_name
        if 'seqid' not in j: raise TProtocolException("TNiceJSONProtocol: bad JSON, missing seqid element")
        j_seqid = j['seqid']
        if (type(j_seqid) is not int): raise TProtocolException("TNiceJSONProtocol: bad JSON, seqid element must be integer")
        self.message_seqid_ = j_seqid
        if 'body' not in j: raise TProtocolException("TNiceJSONProtocol: bad JSON, missing body element")
        ser = None
        if self.message_type_ != TMessageType.EXCEPTION:
            structname = struct_name(self.typelib_, self.service_, self.message_name_, self.message_type_)
            ser = NiceJSON.binary_from_json(self.typelib_, structname, j['body'])
        else:
            body = j['body']
            newbuf = TMemoryBuffer()
            newproto = TBinaryProtocol(TBufferedTransport(newbuf))
            if ('message' not in body) or ('type' not in body):
                x = TApplicationException(TApplicationException.UNKNOWN, "malformed exception " + json.dumps(body))
            else:
                x = TApplicationException(string_to_TApplicationExceptionType(body['type']), body['message'])
            x.wrrite(newproto)
            ser = newbuf.getvalue()

        self.message_buffer_ = TMemoryBuffer(ser)
        self.message_proto_ = TBinaryProtocol(TBufferedTransport(self.message_buffer_))
        self.mode_ = READING_MESSAGE
        return (str(self.message_name_), self.message_type_, self.message_seqid_)

    def readMessageEnd(self):
        self.must_be_reading()
        self.message_buffer_ = None
        self.message_proto_ = None
        self.message_name_ = ''
        self.message_type_ = 0
        self.message_seqid_ = 0
        self.mode_ = MODE_NONE


    def readStructBegin(self):
        self.must_be_reading()
        return self.message_proto_.readStructBegin()

    def readStructEnd(self):
        self.must_be_reading()
        return self.message_proto_.readStructEnd()

    def readFieldBegin(self):
        self.must_be_reading()
        return self.message_proto_.readFieldBegin()

    def readFieldEnd(self):
        self.must_be_reading()
        return self.message_proto_.readFieldEnd()

    def readMapBegin(self):
        self.must_be_reading()
        return self.message_proto_.readMapBegin()

    def readMapEnd(self):
        self.must_be_reading()
        return self.message_proto_.readMapEnd()

    def readListBegin(self):
        self.must_be_reading()
        return self.message_proto_.readListBegin()

    def readListEnd(self):
        self.must_be_reading()
        return self.message_proto_.readListEnd()

    def readSetBegin(self):
        self.must_be_reading()
        return self.message_proto_.readSetBegin()

    def readSetEnd(self):
        self.must_be_reading()
        return self.message_proto_.readSetEnd()

    def readBool(self):
        self.must_be_reading()
        return self.message_proto_.readBool()

    def readByte(self):
        self.must_be_reading()
        return self.message_proto_.readByte()

    def readI16(self):
        self.must_be_reading()
        return self.message_proto_.readI16()

    def readI32(self):
        self.must_be_reading()
        return self.message_proto_.readI32()

    def readI64(self):
        self.must_be_reading()
        return self.message_proto_.readI64()

    def readDouble(self):
        self.must_be_reading()
        return self.message_proto_.readDouble()

    def readString(self):
        self.must_be_reading()
        return self.message_proto_.readString()

    def readBinary(self):
        self.must_be_reading()
        return self.message_proto_.readBinary()

    def readUtf8(self):
        self.must_be_reading()
        return self.message_proto_.readUtf8()

    def skip(self, ttype):
        self.must_be_reading()
        return self.message_proto_.skip(ttype)

class TNiceJSONProtocolFactory(object):
    def __init__(self, typelib, service):
        self.typelib_ = typelib
        self.service_ = service

    def getProtocol(self, trans):
        return TNiceJSONProtocol(trans, self.typelib_, self.service_)
