#include "collection.hpp"
#include "utils.hpp"

#include <assert.h>
#include <sstream>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <iostream>
#include <algorithm>

using namespace std;
using namespace boost::iostreams;

size_t Collection::size(){
    return str_set_.size();
}


void Collection::sort_scores(){
    vector<pair<string,size_t>> zipped;
    zip(str_set_, scores_, zipped);
    std::sort(std::begin(zipped), std::end(zipped), 
        [&](const auto& a, const auto& b)
        {
            return a.second < b.second;
        });

    // Write the sorted pairs back to the original vectors
    unzip(zipped, str_set_, scores_);
}

void Collection::read_collection(const string& file_name, 
                                 const size_t& n_rows,
                                 const bool& skip_header){
    mapped_file mmap(file_name, boost::iostreams::mapped_file::readonly);
    auto file_data = mmap.const_data();
    stringstream ss;
    ss << file_data;
    string line;
    size_t lines_read = 0;
    size_t empty_lines = 0;
    while (std::getline(ss, line)) {
        if (!line.empty()){
            istringstream iss(line);
            string freq, targ_str;
            getline(iss, targ_str, '\t');
            getline(iss, freq, '\t');
            str_set_.push_back(targ_str);
            scores_.push_back(atoi(freq.c_str()));
        }
        else 
            empty_lines ++ ;

        lines_read ++;
        if(skip_header){ // Need to read one extra line including the header
            if(lines_read > n_rows)
                break;
        }
        else {
            if(lines_read == n_rows)
                break;
        }

    }
    if(skip_header){
        str_set_.erase(str_set_.begin());
        scores_.erase(scores_.begin());
    }

    sort_scores(); // Sort scores_ and re-arrange str_set_ accordingly. 
    n_docs_ = str_set_.size();
    cout << "#Strings added to the collection " << n_docs_ 
         << " Empty lines " << empty_lines << "\n";
    assert(str_set_.size() == scores_.size());
}

/* void Collection::sort_keys(){ */
/*     std::sort(str_dict.begin(), str_dict.end(), [](auto &left, auto &right) { */
/*         return left.first < right.first; */
/*     }); */
/* } */

