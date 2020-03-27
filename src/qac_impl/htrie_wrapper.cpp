#include <cassert>
#include <string_view>
#include <utility>
#include <iostream>
#include "htrie_wrapper.hpp"

void HTrieCompleter::build_index(const StringDict& str_dict){
    for(const auto& sv:str_dict){
        ht_map[sv.first] = sv.second;
    }
}

void HTrieCompleter::build_index(const Collection& coll){
    const auto& string_set = coll.get_strings();
    const auto& scores = coll.get_scores();
    for (unsigned int i = 0; i < string_set.size(); ++i) {
       ht_map.insert(string_set[i], scores[i]); 
    }
}

void HTrieCompleter::build_index(const StrVec& str_set, const ScoreVec& scores){
    for (unsigned int i = 0; i < str_set.size(); ++i) {
       ht_map.insert(str_set[i], scores[i]); 
    }
}


CandidateSet<std::string>
HTrieCompleter::complete(const string& prefix, const int& n_comp){
    ch_.reset();
    assert(ch_.n_comp() == 0);
    ch_.set_k(n_comp);
    string key_buffer;
    auto prefix_range = ht_map.equal_prefix_range(prefix);
    for(auto it = prefix_range.first; it != prefix_range.second; ++it){
        /* std::cout << "key " << it.key() << "\t" << "value " << it.value() << "\n"; */
        ch_.insert(it.key(), it.value());
    }
    return ch_.topk_completions();
}

void HTrieCompleter::update_index(const ScoredStr& sc){
    ht_map[sc.first] = sc.second;
}

void HTrieCompleter::print_index_meta(){
    cout << "# string indexed " << ht_map.size() << "\n";
}
