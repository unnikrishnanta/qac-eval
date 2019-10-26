#include "collection.hpp"
#include <sstream>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <iostream>

using namespace std;
using namespace boost::iostreams;

void Collection::read_collection(const string& file_name, 
                                 const size_t& n_rows,
                                 const bool& skip_header){
    mapped_file mmap(file_name, boost::iostreams::mapped_file::readonly);
    auto file_data = mmap.const_data();
    stringstream ss;
    ss << file_data;
    string line;
    size_t lines_read = 0;

    while (std::getline(ss, line)) {
        if(++lines_read >= n_rows)
            break;
        if (!line.empty()){
            istringstream iss(line);
            string freq, targ_str;
            getline(iss, targ_str, '\t');
            getline(iss, freq, '\t');
            str_dict.push_back(make_pair(targ_str, atoi(freq.c_str())));
        }

    }
    if(skip_header)
        str_dict.erase(str_dict.begin());

}
