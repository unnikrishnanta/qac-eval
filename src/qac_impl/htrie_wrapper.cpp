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

StringDict HTrieCompleter::complete(const string& prefix, const size_t& n_comp,
                                    const bool& topk){
    string key_buffer;
    StringDict matches;
    /* matches.reserve(n_comp); */

    auto prefix_range = ht_map.equal_prefix_range(prefix);
    for(auto it = prefix_range.first; it != prefix_range.second; ++it){
        matches.push_back(make_pair(it.key(), it.value()));
    }
    /* cout << matches.size() << " matches found\n"; */
    if(topk)
        sort(matches.begin(), matches.end(), [](auto &left, auto &right) {
            return left.second > right.second;
        });

    /* cout << "# matches here " << matches.size() << "\n"; */
    if(matches.size() > n_comp){
        StringDict completions (matches.begin(), matches.begin() + n_comp);
        return completions;
    }
    return matches;
}

void HTrieCompleter::update_index(const ScoredStr& sc){
    ht_map[sc.first] = sc.second;
}

void HTrieCompleter::print_index_meta(){
    cout << "# string indexed " << ht_map.size() << "\n";
}
