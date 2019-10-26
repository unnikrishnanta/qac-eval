#include <utility>
#include "htrie_wrapper.hpp"

using namespace qac_ht;
void HTrieCompleter::build_index(const sdict_t& str_dict){
    for(const auto& sv:str_dict){
        ht_map[sv.first] = sv.second;
    }
}

sdict_t HTrieCompleter::complete(const string& prefix){
    string key_buffer;
    sdict_t ret_vec;

    for(auto it = ht_map.begin(); it != ht_map.end(); ++it) {
        it.key(key_buffer);
        ret_vec.push_back(make_pair(key_buffer, it.value()));
    }
    
    sort(ret_vec.begin(), ret_vec.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    return ret_vec;
}
