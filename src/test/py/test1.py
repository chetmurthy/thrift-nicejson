import json
import sys
import unittest
import glob
sys.path = ['./gen-py', '../../lib/py/build/lib.linux-x86_64-2.7' ] + sys.path

from test.ttypes import Bar
import thrift_nicejson_binary
from thrift_nicejson import NiceJSON

from thrift import Thrift
from thrift.protocol.TBinaryProtocol import TBinaryProtocol
from thrift.protocol.TJSONProtocol import TJSONProtocol
from thrift.transport import TTransport

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

if __name__ == '__main__':
    unittest.main()
