#include <exception>
#include <string>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>
#include "NiceJSON.h"

struct my_exception : std::exception
{
  char const* what() const throw() { return "One of my exceptions"; }
};

void translate_my_exception(my_exception const& e)
{
    // Use the Python 'C' API to set up an exception object
    PyErr_SetString(PyExc_Exception, e.what());
}

void translate_TException(apache::thrift::TException const& e) {
  PyErr_SetString(PyExc_Exception, e.what());
}

void something_which_throws()
{
    throw my_exception();
}

using apache::thrift::nicejson::NiceJSON ;

const std::string echo(const std::string& name) {
    return std::string("hello, "+name);
}

void prepend_typelib_directory(const std::string& name) {
  NiceJSON::prepend_typelib_directory(name) ;
}

void append_typelib_directory(const std::string& name) {
  NiceJSON::append_typelib_directory(name) ;
}

void install_typelib(const std::string& key, const string& serialized) {
  NiceJSON::install_typelib(key, serialized) ;
}

using namespace apache::thrift::nicejson;

std::string json_from_binary(const std::string& key, const std::string& type, const std::string& serialized) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  json actual = nj->marshal_from_binary(type, (uint8_t*)serialized.data(), serialized.size(), false) ;
  return actual.dump() ;
}

std::string binary_from_json(const std::string& key, const std::string& type, const std::string& json_serialized) {
  NiceJSON const * const nj = NiceJSON::require_typelib(key) ;
  json j = json::parse(json_serialized) ;
  std::string binary = nj->demarshal_to_binary(type, j) ;
  return binary ;
}

void require_typelib(const std::string& key) {
  NiceJSON::require_typelib(key) ;
}

BOOST_PYTHON_MODULE(thrift_nicejson_binary)
{
  using namespace boost::python;
  register_exception_translator<my_exception>(&translate_my_exception);
  register_exception_translator<apache::thrift::TException>(&translate_TException);
  
  def("something_which_throws", something_which_throws);
  def("echo", echo);
  def("install_typelib", install_typelib);
  def("require_typelib", require_typelib);
  def("json_from_binary", json_from_binary);
  def("binary_from_json", binary_from_json);
  def("prepend_typelib_directory", prepend_typelib_directory);
  def("append_typelib_directory", append_typelib_directory);
}
