/*
 * Written by Unni on 27/01/2020/
 *
 * Notes:
    - MAX_QUERY_LENGTH is defined as 64
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
#include <unordered_set>
#include <vector>
#include <string>
#include <getopt.h>
#include <algorithm>

using namespace std;
using namespace dbwsim;


IncNgTrieCompleter::IncNgTrieCompleter() {
    max_tau = 2; // Number of mismatches allowed = 2
}

void IncNgTrieCompleter::build_index(const sdict_t& str_dict){
  vector<string> input_strings;
  std::transform(str_dict.begin(), str_dict.end(),
                 back_inserter(input_strings),
                 [](const auto& p) { return p.first;});
  indextrie = new MapTrie();
  fastss_index = new FastssIndex();
  fastss_index -> Initilization(input_strings, max_tau, false, 0, indextrie);
  active_pool = new ActiveNodePool();
  FastSSSearcher *fsssearcher = new FastSSSearcher();
  fsssearcher -> Initialize(fastss_index, active_pool, max_tau, false);
  searcher = fsssearcher;
}

sdict_t IncNgTrieCompleter::complete(const string& prefix, const size_t& n_comp,
        const bool& topk){
}
