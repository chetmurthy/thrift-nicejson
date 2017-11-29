#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "NiceJSON.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/smart_ptr.hpp>

namespace po = boost::program_options;
using namespace std;
using namespace boost;
using namespace boost::filesystem;

using apache::thrift::nicejson::NiceJSON ;
using apache::thrift::nicejson::file_contents ;

int
main(int ac, char **av) {
    try {
        po::options_description desc("Allowed options");

	string typelib ;
	string type ;
	vector<string> inputfiles ;
	bool permissive = false ;

        desc.add_options()
            ("help", "produce help message")
            ("permissive", po::value<bool>(&permissive), "allow permissive marshalling to JSON")
            ("typelib", po::value<string>(&typelib), "specify typelib file")
            ("type", po::value<string>(&type), "specify type of message")
            ("input-file", po::value< vector<string> >(&inputfiles), "input file")
        ;

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;        
        po::store(po::command_line_parser(ac, av).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help") ||
	    !vm.count("typelib") ||
	    !vm.count("type") ||
	    !vm.count("input-file")
	    ) {
            cout << desc << "\n";
            return 0;
        }

	NiceJSON const * const nj = NiceJSON::require_typelib(typelib) ;
	for(auto ii = inputfiles.begin() ; ii != inputfiles.end() ; ++ii) {
	  if (!exists(*ii)) {
	    cerr << "file " << *ii << " does not exist -- skiping" << endl ;
	    continue ;
	  }
	  cout << "[" << *ii << "]" << endl ;
	  string bytes = file_contents(*ii) ;
	  cout << boost::format{"[%d bytes]\n"} % bytes.size() ;
	  json rv = nj->marshal_from_binary(type, (uint8_t*)bytes.data(), bytes.size(), permissive) ;
	  cout << rv.dump(2) << endl ;
	}

    }
    catch(std::exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}  
