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
#include <sstream>
#include <algorithm>  // for sort
#include <stdexcept>

#include "pqlog.hpp"

using namespace std;
using namespace boost::iostreams;

void PQLog::load_pqlog(const string& file_name, const size_t& n_rows){


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

        if(pq_log.find(qid) == pq_log.end()){ // qid not in the map
            vector<string> tvec;
            pq_log[qid] = tvec;
        }
        
        pq_log[qid].push_back(pq);
        
        }
    }
}


void LRLog::generate_lr_log(const PQLog& pq_log){
    for(const auto& [qid, pvec]: pq_log){
        const auto lastp = pvec.back();
        vector<string> lrp_vec;
        for (int w = 1; w <= lastp.length(); ++w) {
            lrp_vec.push_back(lastp.substr(0, w)); 
        }
        lr_log[qid] = lrp_vec;
    }
}
