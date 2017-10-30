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

const std::string echo(const std::string name) {
    return std::string("hello, "+name);
}

void install_typelib(const std::string package, const std::string name, const string serialized) {
  apache::thrift::nicejson::NiceJSON::install_typelib(package, name, serialized) ;
}

BOOST_PYTHON_MODULE(nicejson)
{
  using namespace boost::python;
  register_exception_translator<my_exception>(&translate_my_exception);
  register_exception_translator<apache::thrift::TException>(&translate_TException);
  
  def("something_which_throws", something_which_throws);
  def("echo", echo);
  def("install_typelib", install_typelib);
}
