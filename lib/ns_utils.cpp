#include <iostream>
#include <iterator>
#include <regex>
#include <streambuf>
#include <string>

#include "ns_utils.h"

string ns_open(const string& ns) {
   std::regex dot_re("\\.");
 
   string rv = std::regex_replace(ns, dot_re, " { namespace ") ;
   return "namespace " + rv + " {" ;
}

string ns_close(const string& ns) {
   std::regex component_re("[^\\.]+\\.?");
 
   string rv = std::regex_replace(ns, component_re, " }") ;
   return rv ;
}
