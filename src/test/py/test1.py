import sys
import glob
sys.path = ['./gen-py'] + sys.path

from test.ttypes import Bar

from thrift import Thrift
from thrift.protocol.TBinaryProtocol import TBinaryProtocol
from thrift.protocol.TJSONProtocol import TJSONProtocol
from thrift.transport import TTransport

def f_write(fname, buf):
    f = open(fname, 'wb')
    f.write(buf)
    f.close()

def main():
    buf = TTransport.TMemoryBuffer()
    transport = TTransport.TBufferedTransportFactory().getTransport(buf)
    protocol = TBinaryProtocol(transport)
#    protocol = TJSONProtocol(transport)

    b = Bar()
    b.a = 1
    b.b = "ugh"
    b.write(protocol)
    protocol.trans.flush()
    ser = buf.getvalue()
    f_write("test1.ser", ser)
    print ser.encode('hex')

if __name__ == '__main__':
    try:
        main()
    except Thrift.TException as tx:
        print('%s' % tx.message)
