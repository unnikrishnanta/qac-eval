#include <iostream>
#include <algorithm>
#include "marisa_wrapper.hpp"
#include "marisa/include/marisa.h"
/* #include "marisa/include/marisa/keyset.h" */
using namespace std;
using namespace qac_mt;

void build_marisa(marisa::Keyset &keyset, const vector<float>& weights,
                  marisa::Trie *trie) {
  for (std::size_t i = 0; i < keyset.size(); ++i) {
    keyset[i].set_weight(weights[i]);
  }
  trie->build(keyset);
  std::printf(" %10lu", (unsigned long)trie->io_size());
}


/* Read the strings and scores in to keyset */
void read_keys(sdict_t string_dict, marisa::Keyset& keyset) {
  for (const auto ss : string_dict) {
    keyset.push_back(ss.first.c_str(), ss.first.length(), ss.second);
  }
}


void MarisaCompleter::build_index(const sdict_t& str_dict){
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


sdict_t MarisaCompleter::complete(const string& prefix,
                                  const size_t& n_comp, const bool& topk){
    keyset.reset();
    agent.set_query(prefix.c_str(), prefix.length());
    while (trie.predictive_search(agent)) {
        keyset.push_back(agent.key());
    }
    if (!keyset.empty()) {
        sdict_t matches;
        /* const std::size_t end = std::min(n_comp, keyset.size()); */
        for (std::size_t i = 0; i < keyset.size(); ++i) {
            auto key_str = string(keyset[i].ptr(), keyset[i].length());
            auto val = weights[keyset[i].id()];
            matches.push_back(make_pair(key_str, val));
        }
        if(topk)
            sort(matches.begin(), matches.end(), [](auto &left, auto &right) {
                return left.second > right.second;
            });
        if(matches.size() > n_comp) {
            sdict_t completions (matches.begin(), matches.begin() + n_comp);
            return completions;
        }
        return matches;
    }
    return {};
}
