#ifndef COLLECTION_H
#define COLLECTION_H

#include<string>
#include<vector>
#include<utility>

#include "dtypes.hpp"

using namespace std;
const string WIKI_ROOT = "../../AutoSuggestions/PyQAC/data/wiki/";
const string CWEB_ROOT = "../../AutoSuggestions/PyQAC/data/"
                            "Clue-500M-Anchor-Log-External/";
const string WIKI_COLLECTION = "clickstream-agg-wiki-64B.tsv";
const string CWEB_COLLECTION = "clueweb-agg-64B.tsv";


class Collection {

    public:
        Collection(const char c) : coll_type(c) {}
        void read_collection(const size_t& n_rows=SIZE_MAX,
                             const bool& skip_header=false);
        /* void sort_keys(); */
        StrVec const &get_strings() const { return str_set_; }
        ScoreVec const &get_scores() const { return scores_; }
        size_t size();

    private:
        const char coll_type; // Wiki or Clueweb
        StrVec str_set_;
        ScoreVec scores_;
        size_t n_docs_; // Number of docs in the collection

        // Sorts scores in ascending order and permutes str_set_ in the
        // corresponding order
        void sort_strdict(int key=0);
};

#endif /* ifndef COLLECTION_H */
