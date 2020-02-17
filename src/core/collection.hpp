#ifndef COLLECTION_H
#define COLLECTION_H

#include<string>
#include<vector>
#include<utility>

#include "dtypes.hpp"

using namespace std;

using sdict_t = vector<pair<string, size_t>>;

class Collection {

    public:
        void read_collection(const string& file_name, 
                             const size_t& n_rows=SIZE_MAX,
                             const bool& skip_header=false);
        /* void sort_keys(); */
        strvec_t const &get_strings() const { return str_set_; }
        scores_t const &get_scores() const { return scores_; }
        size_t size();

    private:
        strvec_t str_set_;
        scores_t scores_;

        // Sorts scores in ascending order and permutes str_set_ in the
        // corresponding order
        void sort_scores();
};

#endif /* ifndef COLLECTION_H */
