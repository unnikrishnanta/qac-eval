/*
 * Written by Unni on 27/01/2020/
 *
 * Notes:
    - MAX_QUERY_LENGTH is defined as 256
 */

#include "IncNgTrie/fastssindex.h"
#include "IncNgTrie/active_node.h"
#include "IncNgTrie/variants.h"
#include "IncNgTrie/dataset.h"
#include "IncNgTrie/resultset.h"
#include "IncNgTrie/searcher_base.h"
#include "IncNgTrie/fastss_searcher.h"
/* #include "IncNgTrie/easy_timer.h" */
#include "IncNgTrie/map_trie.h"
/* #include "IncNgTrie/vector_trie.h" */
/* #include "IncNgTrie/brother_trie.h" */
#include "IncNgTrie_wrapper.hpp"

#include <iostream>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <string>
#include <getopt.h>
#include <algorithm>

using namespace std;
using namespace dbwsim;


IncNgTrieCompleter::IncNgTrieCompleter(int tau){
    max_tau = tau;
}

void IncNgTrieCompleter:: build_index(const StrVec& str_set,
                                      const ScoreVec& scores){
  vector<string> input_strings;
  indextrie = new MapTrie();
  fastss_index = new FastssIndex();
  fastss_index -> Initilization(str_set, scores, max_tau, false, 0, indextrie);
  active_pool = new ActiveNodePool();
  FastSSSearcher *fsssearcher = new FastSSSearcher();
  fsssearcher -> Initialize(fastss_index, active_pool, max_tau, false);
  searcher = fsssearcher;
}

void IncNgTrieCompleter::build_index(const Collection& coll){
    build_index(coll.get_strings(), coll.get_scores());
}


CandidateSet<std::string_view> IncNgTrieCompleter::complete(const string& prefix,
                                        const int& n_comp){
    ch_.reset();
    assert(ch_.n_comp() == 0);
    ch_.set_k(n_comp);
    searcher->ResetSearcher();
    string p = prefix;
    searcher->ExtendQuery(p.data(), p.length());
    searcher->ProcessAll();
    for (auto it = searcher->result_set_.result_ids_.begin();
         it != searcher->result_set_.result_ids_.end(); it ++) {
      auto did = *it;
      const string_view data = searcher->index_->dataset_.GetDocumentByID(did);
      const auto& weight =  searcher->index_->dataset_.GetWeightByID(did);
      ch_.insert(data, weight); 
    }
    return ch_.topk_completions();
}
