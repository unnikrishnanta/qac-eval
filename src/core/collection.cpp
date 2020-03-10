#include "collection.hpp"
#include "dtypes.hpp"
#include "utils.hpp"

#include <assert.h>
#include <sstream>
#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <iostream>
#include <algorithm>
#include <random>

using namespace std;
using namespace boost::iostreams;

Collection::Collection(const char c) : coll_type(c){
    cout << "Collection constructor\n";
}

Collection::~Collection(){
    cout << "Collection destructor\n";
}

size_t Collection::size(){
    return str_dict_.size();
}

void Collection::sort_strdict(StringDict& zipped, int key){
    if(key==0){ // Sort by strings 
        std::sort(std::begin(zipped), std::end(zipped), 
            [&](const auto& a, const auto& b) {
                return a.first < b.first;
            });
    }
    else{ // Sort by scores
        std::sort(std::begin(zipped), std::end(zipped), 
                [&](const auto& a, const auto& b) {
                    return a.second < b.second;
                });
    }
    unzip(zipped);
}

/* Randomly shuffles the vectors str_set and scores and selects nrows samples
 * from them */
void Collection::uniform_sample(const size_t& nrows,
                                const bool& sort, const int& sort_key){
    StringDict sample;
    sample.reserve(nrows);
    std::sample(str_dict_.begin(), str_dict_.end(), std::back_inserter(sample),
                nrows, std::mt19937{std::random_device{}()});
    /* cout << "nrows = " << nrows << " Sampled size " << sample.size() << "\n"; */
    if(sort)
        sort_strdict(sample, sort_key);
    unzip(sample);
    /* cout << "str_set_.size() " << str_set_.size() << "\n"; */
}

/* Reads the tab seperated string score pairs from filename and permutes them
 * according to string if key=0 in sort_strdict and by score otherwise 
 */
void Collection::read_collection(const size_t& n_rows,
                                 const bool& skip_header, 
                                 const bool& sort, const int& sort_key){
    string file_name;
    if(coll_type == 'w')
        file_name = WIKI_ROOT + WIKI_COLLECTION;
    else if(coll_type == 'c')
        file_name = CWEB_ROOT + CWEB_COLLECTION;
    cerr << "Reading " << file_name << "\n";
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

    zip(str_dict_);
    if(sort){ // sort the collection based on sort_key. 0:sort by strings
        sort_strdict(str_dict_, sort_key); 
        unzip(str_dict_);
    }
    assert(str_set_.size() == scores_.size());
    cout << lines_read << " lines read\n";
}

void Collection::read_collection(const bool& skip_header){
    read_collection(SIZE_MAX, skip_header);
}

void Collection::zip(StringDict &zipped)
{
    zipped.reserve(str_set_.size());
    for(size_t i=0; i<str_set_.size(); ++i)
    {
        zipped.push_back(std::make_pair(str_set_[i], scores_[i]));
    }
}

void Collection::unzip(const StringDict& zipped)
{
    str_set_.clear();
    scores_.clear();
    str_set_.reserve(zipped.size());
    scores_.reserve(zipped.size());
    for(const auto& zp: zipped)
    {
        str_set_.push_back(zp.first);
        scores_.push_back(zp.second);
    }
    assert(str_set_.size() == scores_.size());
}





