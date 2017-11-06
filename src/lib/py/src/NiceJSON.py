import json
import thrift_nicejson_binary

def argle():
    print "argle\n"

def prepend_typelib_directory(dir):
    thrift_nicejson_binary.prepend_typelib_directory(dir)

def require_typelib(typelib):
    thrift_nicejson_binary.require_typelib(typelib)

def install_typelib(typelib, serialized):
    thrift_nicejson_binary.install_typelib(typelib, serialized)

def json_from_binary(typelib, ty, ser):
    return json.loads(thrift_nicejson_binary.json_from_binary(typelib, ty, ser))

def binary_from_json(typelib, ty, j):
    js = json.dumps(j)
    return thrift_nicejson_binary.binary_from_json(typelib,ty, js)
