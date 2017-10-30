#include <exception>
#include <string>

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/exception_translator.hpp>

struct my_exception : std::exception
{
  char const* what() const throw() { return "One of my exceptions"; }
};

void translate(my_exception const& e)
{
    // Use the Python 'C' API to set up an exception object
    PyErr_SetString(PyExc_Exception, e.what());
}

void something_which_throws()
{
    throw my_exception();
}

const std::string echo(const std::string name) {
    return std::string("hello, "+name);
}

BOOST_PYTHON_MODULE(nicejson)
{
  using namespace boost::python;
  register_exception_translator<my_exception>(&translate);
  
  def("something_which_throws", something_which_throws);
  def("echo", echo);
}
