/*
   Reads the output from pattern_synth (without post-processing) and loads the 
   test collection on synthetic partial queries.    

 * @Author: Unni Krishnan 
 * @Date: 2019-04-22 23:09:44 
 * @Last Modified by: Unni krishnan
 * @Last Modified time: 2019-10-27
 */

/* #include <boost/iostreams/device/mapped_file.hpp> // for mmap */
/* #include <boost/tokenizer.hpp> */
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>  // for sort
#include <stdexcept>
#include <algorithm>
#include <random>
#include <utility>
#include <cassert>

#include "dtypes.hpp"
#include "pqlog.hpp"
#include "mem_map.hpp"
using namespace std;
/* using namespace boost::iostreams; */

void PQLog::load_qaclog(const string& file_name, const size_t& n_rows){
    cout << "Reading synthetic log from " << file_name << "\n";
    size_t length;
    auto f = map_file(file_name.c_str(), length);
    auto l = f + length;
    const char* newl_f;
    size_t lines = 0;
    while (f && f!=l){
        if ((newl_f = static_cast<const char*>(memchr(f, '\n', l-f)))){
            string f_str = string(f, newl_f-f);
            string::size_type pos = f_str.find('\t');
            if(f_str.npos != pos) {
                auto pq = f_str.substr(0, pos);
                auto qid = f_str.substr(pos + 1);
                /* cout << pq << "\t" << qid << "\n"; */
                if(pq_log_.find(qid) == pq_log_.end()){ // qid not in the map
                    pq_log_[qid] = vector<string>();
                }
                pq_log_[qid].push_back(pq);
            }
            f = newl_f;
            lines++;
            f++;
        }
        if(lines % 10000 == 0)
            std::cerr << "Lines read: " << lines << "\r";
        if(lines == n_rows)
            break;
    }
    cout << lines << " rows read from " << file_name << "\n";
    cout << pq_log_.size() << " conversations\n";
}

PQLog PQLog::lr_log(){
    PQLog lr_log;
    for(const auto& [qid, pvec]: pq_log_){
        const auto lastp = pvec.back();
        vector<string> lrp_vec;
        for (unsigned int w = 1; w <= lastp.length(); ++w) {
            lrp_vec.push_back(lastp.substr(0, w)); 
        }
        lr_log.insert(qid, lrp_vec);
    }
    return lr_log;
}

/* Randomly shuffles the vectors str_set and scores and selects nrows samples
 * from them */
PQLog PQLog::uniform_sample(const size_t& sample_size){

    PQLog sampled_log;
    vector<string> qid_vec, qid_sample;
    std::transform(
        pq_log_.begin(),
        pq_log_.end(),
        std::back_inserter(qid_vec),
        [](const auto &pair){return pair.first;});
    std::sample(qid_vec.begin(), qid_vec.end(), std::back_inserter(qid_sample),
            sample_size, std::mt19937{std::random_device{}()});
    
    for(const auto& qid: qid_sample){
        [[maybe_unused]] auto status = sampled_log.insert(qid, pq_log_[qid]);
        assert(status); // Make sure the sample was not a duplicate
    }
    return sampled_log;
}

bool PQLog::insert(const string& qid, const vector<string>& pvec){
    auto status = pq_log_.insert(std::make_pair(qid, pvec));
    return status.second;
}
