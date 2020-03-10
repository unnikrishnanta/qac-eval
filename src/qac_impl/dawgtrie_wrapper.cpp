/* Wrapper around DAWG Trie Implementation from:
 * https://code.google.com/archive/p/dawgdic/
 * Written By: Unni Krishnan 
 * Date: 14/11/19. 
 */
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <assert.h>
#include "dawgdic/include/dawgdic/ranked-completer.h"
#include "dawgtrie_wrapper.hpp"

using namespace std;

void DAWGTrie::build_index(const StrVec& keys){
    build_status = build(keys);
    assert(build_status);
}

void DAWGTrie::build_index(const Collection& coll){
    build_status = build_index(coll.get_strings(), coll.get_scores());
    assert(build_status);
}

/* Build Trie on keys and values.
 * IMPORTANT: The keys MUST be in dictioary sorted order. 
 * Taken from dawgdic_build.cc
 */
bool DAWGTrie::build_index(const StrVec& keys, const ScoreVec& values) {

    dawgdic::DawgBuilder dawg_builder;
    // Reads keys from an input stream and inserts them into a dawg.
    std::string key;
    /* std::size_t key_count = 0; */
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
            build_status = false;
            assert(build_status);
        }

        /* if (++key_count % 10000 == 0) { */
        /*     std::cerr << "no. keys: " << key_count << '\r'; */
        /* } */
    }

    dawgdic::Dawg dawg;
    if (!dawg_builder.Finish(&dawg)) {
        std::cerr << "error: failed to finish building Dawg" << std::endl;
        build_status = false;
        assert(build_status);
    }
    if (!dawgdic::DictionaryBuilder::Build(dawg, &dic)) {
        std::cerr << "error: failed to build Dictionary" << std::endl;
        build_status = false;
    }
    if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide)) {
        std::cerr << "error: failed to build RankedGuide" << std::endl;
        build_status = false;
        assert(build_status);
    }
    /* Initialise a completer object for the complete() function */
    completer.reset(new dawgdic::RankedCompleter (dic, guide));
    build_status = true;
    assert(build_status);
    return build_status;
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
    if (!dawgdic::RankedGuideBuilder::Build(dawg, dic, &guide)) {
        std::cerr << "error: failed to build RankedGuide" << std::endl;
        return false;
    }
    return true;
}


/* TODO: Check if completer->key() is a local variable that gets destroyed after query??
 */
CandidateSet <std::string> DAWGTrie::complete(const string& prefix, const uint8_t& ncomp) {
    /* RankedCompleter completer(dic, guide); */
    dawgdic::BaseType index = dic.root();
    /* CompHandler<std::string> ch(ncomp); */
    if (dic.Follow(prefix.c_str(), prefix.length(), &index)) {
        completer->Start(index, prefix.c_str(), prefix.length());
        while (completer->Next()) {
            auto key_str = string_view(completer->key(), completer->length());
            /* cout << "key_str " << key_str << "\t"; */
            /* cout << completer->value() << "\n"; */
            /* ch.insert(key_str, completer->value()); */
        }
    }
    /* return ch.topk_completions(); */
    return {};
}

