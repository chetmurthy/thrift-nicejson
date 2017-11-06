import json
import sys
import unittest
import glob
sys.path = ['./gen-py', '../../lib/py/build/lib.linux-x86_64-2.7' ] + sys.path

from test.ttypes import Bar
import nicejson

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
        self.assertEqual("hello, world", nicejson.echo("world"))

class TestDynamicSetup(unittest.TestCase):
    def test_prepend(self):
        nicejson.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "Expected '{'"):
            nicejson.install_typelib("foo", "argle")

    def test_bad_install(self):
        nicejson.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "Expected '{'"):
            nicejson.install_typelib("foo", "argle")

    def test_bad_require(self):
        nicejson.prepend_typelib_directory("../cpp/gen-typelib")
        with self.assertRaisesRegexp(Exception, "error: no typelib foo found on path"):
            nicejson.require_typelib("foo")

    def test_ok_require(self):
        nicejson.prepend_typelib_directory("../cpp/gen-typelib")
        nicejson.require_typelib("apache.thrift.plugin.plugin")
        nicejson.require_typelib("apache.thrift.plugin.plugin")

class TestSer(unittest.TestCase):

    def test_ser_Bar1(self):
        buf = TTransport.TMemoryBuffer()
        transport = TTransport.TBufferedTransportFactory().getTransport(buf)
        protocol = TBinaryProtocol(transport)
        b = Bar()
        b.a = 1
        b.b = "ugh"
        b.write(protocol)
        protocol.trans.flush()
        ser = buf.getvalue()
        nicejson.require_typelib("thrift_test.test")
        js = nicejson.json_from_binary("thrift_test.test", "Bar", ser)
        print js
        self.assertEqual(json.loads('{"a":1,"b":"ugh"}'), json.loads(js))

def main():
    ser = ser_Bar1()
    f_write("test1.ser", ser)
    print ser.encode('hex')

if __name__ == '__main__':
    unittest.main()
