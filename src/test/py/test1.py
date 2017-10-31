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

class TestSer(unittest.TestCase):

    def test_ser_Bar1():
        buf = TTransport.TMemoryBuffer()
        transport = TTransport.TBufferedTransportFactory().getTransport(buf)
        protocol = TBinaryProtocol(transport)
        b = Bar()
        b.a = 1
        b.b = "ugh"
        b.write(protocol)
        protocol.trans.flush()
        ser = buf.getvalue()

def main():
    ser = ser_Bar1()
    f_write("test1.ser", ser)
    print ser.encode('hex')

if __name__ == '__main__':
    unittest.main()
