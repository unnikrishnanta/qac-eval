/* Wrapper around DAWG Trie Implementation from:
 * https://code.google.com/archive/p/dawgdic/
 * Written By: Unni Krishnan 
 * Date: 14/11/19. 
 */
#include <cstddef>
#include <numeric>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>
#include "dawgtrie_wrapper.hpp"

using namespace std;

DAWGTrie::DAWGTrie(const vector<string>& keys, const vector<size_t>& vals):
                        values(vals){
    build_status = build(keys, values);
}

DAWGTrie::DAWGTrie(const vector<string>& keys){
    build_status = build(keys);
}

DAWGTrie::DAWGTrie(const Collection& coll){
    build_status = build(coll.get_strings(), coll.get_scores());
}

/* Build Trie on keys and values.
 * IMPORTANT: The keys MUST be in dictioary sorted order. 
 * Taken from dawgdic_build.cc
 */
bool DAWGTrie::build(const StrVec& keys, const ScoreVec& values) {

    dawgdic::DawgBuilder dawg_builder;
    // Reads keys from an input stream and inserts them into a dawg.
    std::string key;
    std::size_t key_count = 0;
    for (size_t i = 0; i < keys.size(); ++i) {
        static const dawgdic::ValueType MAX_VALUE =
            std::numeric_limits<dawgdic::ValueType>::max();
        long long record = values[i]; // Fixes an invalid record value.
        dawgdic::ValueType value =
            static_cast<dawgdic::ValueType>(record);
        if (record < 0) {
            std::cerr << "warning: negative value is replaced by 0: "
                << record << std::endl; 
            value = 0;
        } else if (record > MAX_VALUE) {
            std::cerr << "warning: too large value is replaced by "
                << MAX_VALUE << ": " << record << std::endl; 
            value = MAX_VALUE;
        }

        if (!dawg_builder.Insert(keys[i].c_str(), keys[i].length(), value)) {
            std::cerr << "error: failed to insert key: "
                << key << std::endl;
            return false;
        }

        if (++key_count % 10000 == 0) {
            std::cerr << "no. keys: " << key_count << '\r';
        }
    }

    dawgdic::Dawg dawg;
    if (!dawg_builder.Finish(&dawg)) {
        std::cerr << "error: failed to finish building Dawg" << std::endl;
        return false;
    }
    if (!dawgdic::DictionaryBuilder::Build(dawg, &dic)) {
        std::cerr << "error: failed to build Dictionary" << std::endl;
        return false;
    }
    if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide)) {
        std::cerr << "error: failed to build RankedGuide" << std::endl;
        return false;
    }
    /* Initialise a completer object for the complete() function */
    completer.reset(new dawgdic::RankedCompleter (dic, guide));
    return true;
}


bool DAWGTrie::build(const StrVec& keys) {
    dawgdic::DawgBuilder builder;
    std::vector<size_t> vals(keys.size()); // vector with 100 ints.
    std::iota (std::begin(vals), std::end(vals), 0); // Fill with 0, 1, ..., 99.

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
    /* if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide, */
    /*             Comparer(vals))) { */
    /*     std::cerr << "error: failed to build RankedGuide" << std::endl; */
    /*     return false; */
    /* } */
    if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide)) {
        std::cerr << "error: failed to build RankedGuide" << std::endl;
        return false;
    }
    /* Initialise a completer object for the complete() function */
    completer.reset(new dawgdic::RankedCompleter (dic, guide));
    return true;
}

StringDict DAWGTrie::complete(const string& prefix, const size_t& ncomp) {
    char p;
    dawgdic::BaseType index; 
    index = dic.root();
    for(auto i=0; i<prefix.length(); ++i){
        p = prefix[i];
        if(!dic.Follow(p, &index))
            continue;
    }    
    completer->Start(index, &p, 1);
    StringDict comps; 
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
