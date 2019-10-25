#include "collection.hpp"
#include <sstream>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <iostream>

using namespace std;
using namespace boost::iostreams;

void Collection::read_collection(const string& file_name){
    mapped_file mmap(file_name, boost::iostreams::mapped_file::readonly);
    auto file_data = mmap.const_data();
    stringstream ss;
    ss << file_data;
    string line;

    while (std::getline(ss, line)) {
        if (!line.empty()){
            istringstream iss(line);
            string freq, targ_str;
            getline(iss, targ_str, '\t');
            getline(iss, freq, '\t');
            str_dict.push_back(make_pair(targ_str, atoi(freq.c_str())));
        }

    }
}
