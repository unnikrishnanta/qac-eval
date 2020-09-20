#include "collection.hpp"
#include "dtypes.hpp"
#include "utils.hpp"
#include "mem_map.hpp"

#include <assert.h>
#include <sstream>
/* #include <boost/iostreams/device/mapped_file.hpp> // for mmap */
#include <iostream>
#include <algorithm>
#include <random>
// for mmap:
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utility>

using namespace std;
/* using namespace boost::iostreams; */


size_t Collection::size() const{
    return str_set_.size(); 
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
    /* unzip(zipped); */
}

/* Randomly shuffles the vectors str_set and scores and selects nrows samples
 * from them */
pair<StrVec, ScoreVec> Collection::uniform_sample(const size_t& nrows,
                                const bool& sort, const int& sort_key){
    if (nrows >= str_set_.size())
        return make_pair(get_strings(), get_scores());
    StringDict sample;
    sample.reserve(nrows);
    std::sample(str_dict_.begin(), str_dict_.end(), std::back_inserter(sample),
                nrows, std::mt19937{std::random_device{}()});
    if(sort)
        sort_strdict(sample, sort_key);

    StrVec sample_strs; 
    ScoreVec sample_scores;
    for (const auto zp : sample) {
       sample_strs.push_back(zp.first); 
       sample_scores.push_back(zp.second);
    }
    
    return std::make_pair(sample_strs, sample_scores);
}

void Collection::zip(StringDict &zipped)
{
    zipped.reserve(str_set_.size());
    for(size_t i=0; i<str_set_.size(); ++i)
    {
        zipped.push_back(std::make_pair(str_set_[i], scores_[i]));
    }
}

Collection Collection::unzip(const StringDict& zipped, bool inplace)
{
    
    if(inplace){
        cout << "Sampling in place" << "\n";
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
        return {};
    }
    else {
        Collection sampled_coll;
        sampled_coll.read_collection(zipped);
        return sampled_coll;
    }
}

/*
    Reads a log file from file_name. The log files have the format:
    partial-query   query_id

    Parameters:
    -----------
    file_name: The file name containing the log in the above format.
    n_lines: If n_lines = 0, all the lines from the file will be read.
             Otherwise read only n_lines from the file.
*/
void Collection::read_collection(const char& coll_type, const size_t& n_rows,
                                 const bool& skip_header, 
                                 const bool& sort, const int& sort_key){
    string file_name;
    if(coll_type == 'w')
        file_name = WIKI_ROOT + WIKI_COLLECTION;
    else if(coll_type == 'c')
        file_name = CWEB_ROOT + CWEB_COLLECTION;
    else if(coll_type == 'b')
    file_name = BING_ROOT + BING_COLLECTION;
    read_collection(file_name, n_rows, skip_header, sort, sort_key);
    coll_type_ = coll_type;
}

void Collection::read_collection(const string& file_name, const size_t& n_rows,
                                 const bool& skip_header, 
                                 const bool& sort, const int& sort_key){
    cerr << "Reading " << file_name << "\n";
    size_t length;
    auto f = map_file(file_name.c_str(), length);
    auto l = f + length;
    const char* newl_f;
    uintmax_t lines_read = 0;  // number of lines counted in the functon
    while (f && f!=l){
        if ((newl_f = static_cast<const char*>(memchr(f, '\n', l-f)))){
            string f_str = string(f, newl_f-f);
            string::size_type pos = f_str.find('\t');
            if(f_str.npos != pos) {
                auto targ_str = f_str.substr(0, pos);
                auto freq = f_str.substr(pos + 1);
                /* cout << targ_str << "\t" << freq << "\n"; */
                str_set_.push_back(targ_str);
                scores_.push_back(atoi(freq.c_str()));
            }
            f = newl_f;
            lines_read++;
            f++;
            if(skip_header){ // Need to read one extra line including the header
                if(lines_read > n_rows)
                    break;
            }
            else {
                if(lines_read == n_rows)
                    break;
            }
        }
        if(lines_read % 10000 == 0)
            std::cerr << "Lines read: " << lines_read << "\r";
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
    cout << lines_read << " lines read \n";
}



void Collection::read_collection(const StrVec& strs, const ScoreVec& scores,
                             const bool sort, const int& sort_key){
    assert(strs.size() == scores.size());
    str_set_.clear(); 
    scores_.clear(); 
    str_set_.reserve(strs.size());
    scores_.reserve(scores.size());
    for (const auto s : strs) {
        str_set_.push_back(s);
    }
    for (const auto sc : scores) {
        scores_.push_back(sc);
    }
    zip(str_dict_);
    if(sort){ // sort the collection based on sort_key. 0:sort by strings
        sort_strdict(str_dict_, sort_key); 
        unzip(str_dict_);
    }
    assert(str_set_.size() == scores_.size());
}

void Collection::read_collection(const StringDict& sdict, const bool sort,
                     const int& sort_key){
    str_set_.clear(); 
    scores_.clear(); 
    str_set_.reserve(sdict.size());
    scores_.reserve(sdict.size());

    for (const auto zp : sdict) {
        str_set_.push_back(zp.first);
        scores_.push_back(zp.second);
    }
    zip(str_dict_);
    if(sort){ // sort the collection based on sort_key. 0:sort by strings
        sort_strdict(str_dict_, sort_key); 
        unzip(str_dict_);
    }
    assert(str_set_.size() == scores_.size());
}
