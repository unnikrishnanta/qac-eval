// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie. 

#ifndef __RESULTSET_H__
#define __RESULTSET_H__

//#include <unordered_set>
#include "cstddef"
#include <cstddef>
#include <unordered_set>

using namespace std;

namespace dbwsim{

class ResultSet {
 public:
  unordered_set<size_t> result_ids_;  // Unni int -> size_t
  //set<int> result_ids_;
  
  void add (size_t document_id) { // Unni int -> size_t
    result_ids_.insert(document_id);
  }
  
  void Clear() {
    result_ids_.clear();
  }

  size_t size() { // Unni int -> size_t
    return result_ids_.size();
  } 
};

}

#endif //__RESULTSET_H__
