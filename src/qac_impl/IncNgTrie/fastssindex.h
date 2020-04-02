// Copyright 2012 UNSW CSE. All Rights Reserved.
// Author: jqin@cse.unsw.edu.au (Jianbin Qin)

// This module use the most naive data structure to
// construct a trie.

#ifndef __FASTSSINDEX_H__
#define __FASTSSINDEX_H__

//#include "naive_trie.h"
#include "variants.h"
#include "dataset.h"
#include "trie_base.h"
#include "map_trie.h"
#include "brother_trie.h"
#include "dtypes.hpp"

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

namespace dbwsim {

struct VariantTuple{
  string variant_string;
  int document_id;
  int next; // Connect to next did with the same last_del_pos
};

class FastssIndex {
 public:
  RawDataSet dataset_;
  //NaiveTrie trie_index_;
  TrieBase* trie_;
  VariantGenerator variant_generator_;
  VariantTuple* variant_tuples;
  SizeType num_variant_tuples;
  BrotherTrie *temptrie;

  // Empty index.
  FastssIndex(){
    trie_ = NULL;
    temptrie = NULL;
  }

  /* Added by Unni */
  void Initilization(const vector<string>& input_strings, 
                     const vector<size_t>& scores, int tau,
                     bool normalized, int norm_gap, TrieBase *trie);

  void Initilization(const string &filename, int tau = 3,
                     bool normalized = false, int norm_gap = 5,
                     TrieBase *trie = NULL);

  void FastInit(const string &filename, int tau = 3,
                     bool normalized = false, int norm_gap = 5,
                     TrieBase *trie = NULL);

  void DumpIndex(bool print_trie = false){
    for (auto i = 0; i < num_variant_tuples; i ++){
      cout << i << ":" <<  variant_tuples[i].variant_string << " "
           << variant_tuples[i].document_id << " " << variant_tuples[i].next << endl;
    }
    if (print_trie) {
      cout << "********************* Index trie *********************" <<endl;
      trie_->PrintTrie();
    }
    TrieStatistic triestate;
    triestate.Clear();
    triestate.GatherStatistic(trie_);
    cerr << triestate.ToString();
  }
};

}

#endif //__FASTSSINDEX_H__
