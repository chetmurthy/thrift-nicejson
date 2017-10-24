#include <iostream>
#include <iterator>
#include <regex>
#include <streambuf>
#include <string>

#ifndef NS_UTIL_INCLUDED
#define NS_UTIL_INCLUDED

using std::string ;

string ns_open(const string& ns);

string ns_close(const string& ns);

string ns_prefix(const string& ns) ;

#endif
