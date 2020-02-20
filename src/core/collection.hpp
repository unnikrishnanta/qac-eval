#ifndef COLLECTION_H
#define COLLECTION_H

#include<string>
#include<vector>
#include<utility>

#include "dtypes.hpp"

using namespace std;

class Collection {

    public:
        void read_collection(const string& file_name, 
                             const size_t& n_rows=SIZE_MAX,
                             const bool& skip_header=false);
        /* void sort_keys(); */
        StrVec const &get_strings() const { return str_set_; }
        ScoreVec const &get_scores() const { return scores_; }
        size_t size();

    private:
        StrVec str_set_;
        ScoreVec scores_;
        size_t n_docs_; // Number of docs in the collection

        // Sorts scores in ascending order and permutes str_set_ in the
        // corresponding order
        void sort_scores();
};

#endif /* ifndef COLLECTION_H */
