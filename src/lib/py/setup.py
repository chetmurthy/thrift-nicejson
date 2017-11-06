from os import environ
from distutils.core import setup, Extension
 
# define the name of the extension to use
extension_name    = 'thrift_nicejson'
extension_version = '1.0'
 
# define the directories to search for include files
# to get this to work, you may need to include the path
# to your boost installation. Mine was in 
# '/usr/local/include', hence the corresponding entry.
include_dirs = [ '/usr/include' , '../cpp' , environ['thrift_INCLUDEDIR'] ]
 
# define the library directories to include any extra
# libraries that may be needed.  The boost::python
# library for me was located in '/usr/local/lib'
library_dirs = [ '../cpp' , '/usr/lib' , environ['thrift_LIBDIR'] ]
 
# define the libraries to link with the boost python library
libraries = [ 'nicejson', 'boost_python', 'boost_system', 'boost_filesystem' , 'thrift' ]
 
# define the source files for the extension
source_files = [ 'thrift_nicejson.cpp' ]
 
# create the extension and add it to the python distribution
setup(
    name='thrift_nicejson',
    version='1.0',
    packages = [
        'thrift_nicejson'
    ],
    package_dir={ 'thrift_nicejson' : 'src' },
    ext_modules=[Extension('thrift_nicejson_binary',
        [ 'thrift_nicejson.cpp' ],
        include_dirs=[ '/usr/include' , '../cpp' , environ['thrift_INCLUDEDIR'] ],
        library_dirs=[ '../cpp' , '/usr/lib' , environ['thrift_LIBDIR'] ],
        libraries=[ 'nicejson', 'boost_python', 'boost_system', 'boost_filesystem' , 'thrift' ],
        extra_compile_args=['-std=gnu++11'],
        extra_link_args=[ '-Wl,-rpath,' + environ['thrift_LIBDIR'] ]
   )]
)
