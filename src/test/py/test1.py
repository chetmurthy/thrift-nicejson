import string
import json
import sys
import unittest
import glob
sys.path = ['./gen-py', '../../lib/py/build/lib.linux-x86_64-2.7' ] + sys.path

from test.ttypes import Bar
import thrift_nicejson_binary
from thrift_nicejson import NiceJSON
from thrift_nicejson import TNiceJSONProtocol

from thrift import Thrift
from thrift.protocol.TBinaryProtocol import TBinaryProtocol
from thrift.protocol.TJSONProtocol import TJSONProtocol
from thrift.transport import TTransport
from thrift.Thrift import TMessageType, TException, TApplicationException

def f_write(fname, buf):
    f = open(fname, 'wb')
    f.write(buf)
    f.close()

def f_contents(fname):
    with open(fname, 'r') as f: s = f.read()
    return s

class TestEcho(unittest.TestCase):
    def test_echo(self):
        self.assertEqual("hello, world", thrift_nicejson_binary.echo("world"))

class TestDynamicSetup(unittest.TestCase):
    def test_prepend(self):
        NiceJSON.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "Expected '{'"):
            NiceJSON.install_typelib("foo", "argle")

    def test_bad_install(self):
        NiceJSON.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "Expected '{'"):
            NiceJSON.install_typelib("foo", "argle")

    def test_bad_require(self):
        NiceJSON.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "error: no typelib foo found on path"):
            NiceJSON.require_typelib("foo")

    def test_ok_require(self):
        NiceJSON.prepend_typelib_directory("../cpp/gen-typelib")
        NiceJSON.require_typelib("apache.thrift.plugin.plugin")
        NiceJSON.require_typelib("apache.thrift.plugin.plugin")

    def test_struct_names(self):
        self.assertEqual('S2_foo_args', NiceJSON.service_struct_name_args("thrift_test.test", "S2", "foo"))
        self.assertEqual('S2_foo_result', NiceJSON.service_struct_name_result("thrift_test.test", "S2", "foo"))

class TestSer(unittest.TestCase):

    def test_ser_Bar1_again(self):
        buf = TTransport.TMemoryBuffer()
        transport = TTransport.TBufferedTransportFactory().getTransport(buf)
        protocol = TBinaryProtocol(transport)
        b = Bar()
        b.a = 1
        b.b = "ugh"
        b.write(protocol)
        protocol.trans.flush()
        ser = buf.getvalue()
        self.roundtrip("thrift_test.test", "Bar", ser, json.loads('{"a":1,"b":"ugh"}'))

    def roundtrip(self, typelib, ty, ser, expected):
        NiceJSON.require_typelib(typelib)
        js = NiceJSON.json_from_binary(typelib, ty, ser)
        self.assertEqual(expected, js)
        ser2 = NiceJSON.binary_from_json(typelib,ty, js)
        self.assertEqual(ser, ser2)

import json

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

class TestBuffering(unittest.TestCase):
    def setup(self, msg):
        buf = TTransport.TMemoryBuffer(msg)
        transport = TTransport.TBufferedTransport(buf, 4)
        return (buf, transport)

    def dotest(self, msg, j):
        (buf, trans) = self.setup(msg)
        j2 = readjson(trans)
        print ("dotest: j=%s" % j2)
        self.assertEqual(j, j2)

    def test1(self):
        self.dotest("{}", {})

    def test2(self):
        with self.assertRaises(EOFError):
            self.dotest("{        ", {})

    def dotest2(self, msg, j, wantc):
        (buf, trans) = self.setup(msg)
        self.assertEqual(j, readjson(trans))
        readc = trans.cstringio_buf.read(1)
        if readc == '':
            trans.cstringio_refill('', 1)
            readc = trans.cstringio_buf.read(1)
        self.assertEqual(wantc, readc)

    def test3(self):
        self.dotest2("{}a", {}, "a")

    def test4(self):
        for i in range(0, 40):
            txt = "{ " + string.ljust("", i, ' ') + "}a"
            self.dotest2(txt, {}, "a")

    def test5(self):
        self.dotest('{"a": 1}', { 'a': 1 })

from test.S2 import ping_args, foo_args

class TestProtocol(unittest.TestCase):
    def readtest1(self, typelib, service, msg, obj):
        buf = TTransport.TMemoryBuffer(msg)
        transport = TTransport.TBufferedTransport(buf)
        proto = TNiceJSONProtocol.TNiceJSONProtocol(transport, typelib, service)
        (name, type, seqid) = proto.readMessageBegin()
        obj.read(proto)
        proto.readMessageEnd()
        return (name, type, seqid)

    def writetest1(self, typelib, service, msgname, msgty, msgseqid, obj):
        buf = TTransport.TMemoryBuffer()
        transport = TTransport.TBufferedTransport(buf)
        proto = TNiceJSONProtocol.TNiceJSONProtocol(transport, typelib, service)
        proto.writeMessageBegin(msgname, msgty, msgseqid)
        obj.write(proto)
        proto.writeMessageEnd()
        transport.flush()
        return buf.getvalue()

    def test1read(self):
        obj = ping_args()
        self.assertEqual( ('ping', TMessageType.CALL, 0),
                          self.readtest1("thrift_test.test", "S2",'{"body":{},"name":"ping","seqid":0,"type":"call"}', obj) )

    def test1write(self):
        obj = ping_args()
        self.assertEqual( {"body":{},"name":"ping","seqid":0,"type":"call"},
                          json.loads(self.writetest1("thrift_test.test", "S2", 'ping', TMessageType.CALL, 0, obj)) )

    def test2write(self):
        obj = foo_args(42, Bar(32, "ugh"))
        self.assertEqual( {"body": {"w": {"a": 32, "b": "ugh"}, "n": 42}, "seqid": 0, "type": "call", "name": "foo"},
                          json.loads(self.writetest1("thrift_test.test", "S2", 'foo', TMessageType.CALL, 0, obj)))

if __name__ == '__main__':
    unittest.main()
