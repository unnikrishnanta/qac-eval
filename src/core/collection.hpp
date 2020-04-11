#ifndef COLLECTION_H
#define COLLECTION_H

#include<string>
#include<vector>
#include<utility>
#include "dtypes.hpp"
#include "config.hpp"


class Collection {

    public:
        void read_collection(const char& coll_type, const size_t& n_rows,
                             const bool& skip_header=false, 
                             const bool& sort=true, const int& sort_key=0);
        void read_collection(const string& file_name, const size_t& n_rows,
                             const bool& skip_header=false, 
                             const bool& sort=true, const int& sort_key=0);
        void read_collection(const bool& skip_header=false);
        /* void sort_keys(); */
        StrVec const &get_strings() const { return str_set_; }
        ScoreVec const &get_scores() const { return scores_; }
        size_t size();  /* Current size of the collection after sampling */

        void uniform_sample(const size_t& nrows,
                            const bool& sort=true, const int& sort_key=0);

    private:
        StrVec str_set_; // If sampled, only the sample will be stored
        ScoreVec scores_; // If sampled, only the sample will be stored
        StringDict str_dict_; // Always stores the collection in full
        // Sorts scores in ascending order and permutes str_set_ in the
        // corresponding order
        void sort_strdict(StringDict& zipped, int key=0);
        void zip(StringDict& zipped);
        void unzip(const StringDict& zipped);
};

#endif /* ifndef COLLECTION_H */
