/*
   Reads the output from pattern_synth (without post-processing) and loads the 
   test collection on synthetic partial queries.    

 * @Author: Unni Krishnan 
 * @Date: 2019-04-22 23:09:44 
 * @Last Modified by: Unni krishnan
 * @Last Modified time: 2019-10-27
 */

#include <boost/iostreams/device/mapped_file.hpp> // for mmap
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <algorithm>  // for sort
#include <stdexcept>
#include <algorithm>
#include <random>
#include <utility>

#include "dtypes.hpp"
#include "pqlog.hpp"

using namespace std;
using namespace boost::iostreams;

void PQLog::load_pqlog(const string& file_name, const size_t& n_rows){


  cout << "Reading synthetic log from " << file_name << "\n";
  mapped_file mmap(file_name, mapped_file::readonly);
  auto file_data = mmap.const_data();
  stringstream ss;
  ss << file_data;
  string line;
  size_t lines = 0;

  while (std::getline(ss, line)) {
      if(++lines == n_rows)
          break;

      if (!line.empty()){
        istringstream iss(line);
        string pq, qid; 
        getline(iss, pq, '\t');
        getline(iss, qid, '\t');

        if(pq_log_.find(qid) == pq_log_.end()){ // qid not in the map
            pq_log_[qid] = vector<string>();
        }
        
        pq_log_[qid].push_back(pq);
        
        }
    }
    cout << lines << " rows read from " << file_name << "\n";
    cout << pq_log_.size() << " conversations\n";
}

QueryLog PQLog::lr_log(){
    QueryLog lr_log;
    for(const auto& [qid, pvec]: pq_log_){
        const auto lastp = pvec.back();
        vector<string> lrp_vec;
        for (unsigned int w = 1; w <= lastp.length(); ++w) {
            lrp_vec.push_back(lastp.substr(0, w)); 
        }
        lr_log[qid] = lrp_vec;
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
        auto status = sampled_log.insert(qid, pq_log_[qid]);
        assert(status); // Make sure the sample was not a duplicate
    }
    return sampled_log;
}

bool PQLog::insert(const string& qid, const vector<string>& pvec){
    auto status = pq_log_.insert(std::make_pair(qid, pvec));
    return status.second;
}
