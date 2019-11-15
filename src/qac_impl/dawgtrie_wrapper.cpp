/* Wrapper around DAWG Trie Implementation from:
 * https://code.google.com/archive/p/dawgdic/
 * Written By: Unni Krishnan 
 * Date: 14/11/19. 
 */
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>
#include "dawgtrie_wrapper.hpp"

using namespace std;

DAWGTrie::DAWGTrie(const vector<string>& keys, const vector<size_t>& vals):
                        values(vals){
    build(keys, values);
}

/* Build Trie on keys and values.
 * IMPORTANT: To save time, pre-sort the vectors in INCREASING  value order. 
 */
bool DAWGTrie::build(const vector<string>& keys,
                     const vector<size_t>& vals) {
    dawgdic::DawgBuilder builder;
    for (std::size_t i = 0; i < keys.size(); ++i) {
        if (!builder.Insert(keys[i].c_str(), 
                    static_cast<dawgdic::ValueType>(vals[i]))) {
            std::cerr << "error: failed to insert key: "
                << keys[i] << std::endl;
            return false;
        }
    }
    dawgdic::Dawg dawg;
    if (!builder.Finish(&dawg)) {
        std::cerr << "error: failed to finish building Dawg" << std::endl;
        return false;
    }
    if (!dawgdic::DictionaryBuilder::Build(dawg, &dic)) {
        std::cerr << "error: failed to build Dictionary" << std::endl;
        return false;
    }
    if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide,
                Comparer(vals))) {
        std::cerr << "error: failed to build RankedGuide" << std::endl;
        return false;
    }
    /* Initialise a completer object for the complete() function */
    completer.reset(new dawgdic::RankedCompleterBase<Comparer>
                        (dic, guide, Comparer(values)));
    return true;
}


sdict_t DAWGTrie::complete(const string& prefix, const size_t& ncomp) {
    char p;
    dawgdic::BaseType index; 
    index = dic.root();
    for(auto i=0; i<prefix.length(); ++i){
        p = prefix[i];
        if(!dic.Follow(p, &index))
            continue;
    }    
    completer->Start(index, &p, 1);
    sdict_t comps; 
    if(ncomp != SIZE_MAX)
        comps.reserve(ncomp);
    size_t k=0;
    while (completer->Next()) {
        if(k++ >= ncomp)
            break;
        comps.push_back(make_pair(completer->key(), completer->value()));
    }
    return comps;
}


/* int main(int argc, char *argv[]) */
/* { */
/*     vector<string> keys = {"abc", "abcd", "ccd"}; */
/*     vector<size_t> values = {1,2,3}; */
/*     DAWGTrie dtrie (keys, values); */
/*     auto comps = dtrie.complete("a",2); */
/*     for(const auto& kv: comps) */
/*         cout << kv.first << "\t" << kv.second << "\n"; */
/*     return 0; */
/* } */
