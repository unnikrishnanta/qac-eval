#include <iostream>
#include <algorithm>
#include "marisa_wrapper.hpp"
#include "marisa/include/marisa.h"
/* #include "marisa/include/marisa/keyset.h" */
using namespace std;

/* void build_marisa(marisa::Keyset &keyset, const vector<float>& weights, */
/*                   marisa::Trie *trie) { */
/*   for (std::size_t i = 0; i < keyset.size(); ++i) { */
/*     keyset[i].set_weight(weights[i]); */
/*   } */
/*   trie->build(keyset); */
/*   std::printf(" %10lu", (unsigned long)trie->io_size()); */
/* } */


/* Read the strings and scores in to keyset */
void read_keys(StringDict string_dict, marisa::Keyset& keyset) {
  for (const auto ss : string_dict) {
    keyset.push_back(ss.first.c_str(), ss.first.length(), ss.second);
  }
}

/* The above function overloaded */
void read_keys(const StrVec& str_set, const ScoreVec& scores ,
               marisa::Keyset& keyset) {
  for (unsigned int i=0; i < str_set.size(); ++i) {
    keyset.push_back(str_set[i].c_str(), str_set[i].length(), scores[i]);
  }
}


void MarisaCompleter::build_index(const StringDict& str_dict){
    keyset.reset();
    read_keys(str_dict, keyset);
    trie.build(keyset);
    // Store weights in the weights array. See how weights are managed
    // https://code.google.com/archive/p/marisa-trie/issues/4
    weights.resize(keyset.size());
    for(size_t i=0; i< keyset.size(); ++i){
        auto k = keyset[i];
        weights[k.id()] = str_dict[i].second;
    }
    /* std::printf(" %10lu\n", (unsigned long)trie.io_size()); */
}

void MarisaCompleter::build_index(const Collection& coll){
    build_index(coll.get_strings(), coll.get_scores());
}

void MarisaCompleter::build_index(const StrVec& str_set, const ScoreVec& scores){
    keyset.reset();
    read_keys(str_set, scores, keyset);
    trie.build(keyset);
    // Store weights in the weights array. See how weights are managed
    // https://code.google.com/archive/p/marisa-trie/issues/4
    weights.resize(keyset.size());
    for(size_t i=0; i< keyset.size(); ++i){
        auto k = keyset[i];
        weights[k.id()] = scores[i];
    }
}

void MarisaCompleter::predictive_search(const string& str,
                                        const size_t& max_num_results){
    keyset.reset();
    agent.set_query(str.c_str(), str.length());
    while (trie.predictive_search(agent)) {
        keyset.push_back(agent.key());
    }
    if (keyset.empty()) {
        std::cout << "not found" << std::endl;
    } else {
        std::cout << keyset.size() << " found" << std::endl;
        const std::size_t end = std::min(max_num_results, keyset.size());
        for (std::size_t i = 0; i < end; ++i) {
            std::cout << keyset[i].id() << '\t';
            std::cout.write(keyset[i].ptr(),
            static_cast<std::streamsize>(keyset[i].length())) << '\t';
                std::cout << weights[keyset[i].id()] << '\n';
        }
    }
    /* keyset.reset(); */
}


CandidateSet<std::string_view> MarisaCompleter::complete(const string& prefix,
                                     const uint8_t& n_comp){
    keyset.reset();
    agent.set_query(prefix.c_str(), prefix.length());
    while (trie.predictive_search(agent)) {
        keyset.push_back(agent.key());
    }
    if (!keyset.empty()) {
        CompHandler<std::string_view> ch(n_comp);
        for (std::size_t i = 0; i < keyset.size(); ++i) {
            auto key_str = string_view(keyset[i].ptr(), keyset[i].length());
            auto score = weights[keyset[i].id()];
            ch.insert(key_str, score);
        }
        return ch.topk_completions();
    }
    return {};
}
