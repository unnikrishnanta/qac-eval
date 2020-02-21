// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie.

#include "dataset.h"
#include "../../core/utils.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <numeric>

using namespace std;

namespace dbwsim {


void RawDataSet::InitFromFile(const string& filename,
                              int max_length)
{
  int total_length = 0;
  ifstream infile;
  infile.open(filename.c_str());
  max_length_ = max_length;
  char *line = new char[max_length_];
  infile.getline(line, max_length_);
  num_documents_ = 0;
  documents_.clear();
  while (infile.good()){
    string linestr = line;
    documents_.push_back(linestr);
    num_documents_ ++;
    total_length += linestr.length();
    infile.getline(line, max_length_);
  }
  average_length_ = num_documents_ > 0 ? total_length/num_documents_ : 0;
  sort(documents_.begin(), documents_.end());
  delete[] line;
}

void RawDataSet::InitFromStrings(const vector<string>& input_strings,
                                 const vector<size_t>& scores, int max_length)
{
  int total_length = 0;
  max_length_ = max_length;
  num_documents_ = 0;
  /* vector<string>::iterator it; */
  documents_.reserve(input_strings.size());
  for (const auto& s: input_strings)
  {
    documents_.push_back(s);
    num_documents_ ++;
    total_length += s.length();
  }
  average_length_ = num_documents_ > 0 ? total_length/num_documents_ : 0;

  weights_.reserve(num_documents_);
  for (const auto sc: scores) {
     weights_.push_back(sc); 
  }

  /* sort(documents_.begin(), documents_.end());
   * This line from the original source is not required anymore since the
   * defualt sort order is set to string sorted order. 
   */
}

RawDataSet::RawDataSet(const string& filename, int max_length)
{
  InitFromFile(filename, max_length);
}

RawDataSet::RawDataSet(vector<string>& input_strings,
                       const vector<size_t>& scores, int max_length)
{
  InitFromStrings(input_strings, scores,  max_length);
}

void RawDataSet::DumpDataset(ostream& out)
{
  int id = 0;
  for (vector<string>::iterator it = documents_.begin();
       it != documents_.end(); it ++)
  {
    out << id++ << ": " << *it << endl;
  }
}


const string& RawDataSet::GetDocumentByID(int id){
  return documents_[id];
}


const size_t& RawDataSet::GetWeightByID(int id){
    return weights_[id];
}

} // End of namespace
