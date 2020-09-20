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
        void read_collection(const StrVec& strs, const ScoreVec& scores,
                             const bool sort=true, const int& sort_key=0);
        void read_collection(const StringDict& sdict, const bool sort=true,
                             const int& sort_key=0);
        /* void sort_keys(); */
        StrVec const &get_strings() const { return str_set_; }

        StrVec get_strings(const size_t& n) const {
            if(n != SIZE_MAX) {  // return a sub vector
                StrVec str_subvec (str_set_.begin(), str_set_.begin()+n);
                return str_subvec;
            }
            return str_set_;
        }

        ScoreVec const &get_scores() const { return scores_; }
        ScoreVec get_scores(const size_t& n) const {
            if(n != SIZE_MAX) {  // return a sub vector
                ScoreVec score_subvec (scores_.begin(), scores_.begin()+n);
                return score_subvec;
            }
            return scores_;
        }
        size_t size() const;  /* Current size of the collection after sampling */

        char coll_type() const { return std::as_const(coll_type_); }

        pair<StrVec, ScoreVec> uniform_sample(const size_t& nrows,
                            const bool& sort=true, const int& sort_key=0);

    private:
        StrVec str_set_; // If sampled, only the sample will be stored
        ScoreVec scores_; // If sampled, only the sample will be stored
        StringDict str_dict_; // Always stores the collection in full
        char coll_type_;
        // Sorts scores in ascending order and permutes str_set_ in the
        // corresponding order
        void sort_strdict(StringDict& zipped, int key=0);
        void zip(StringDict& zipped);
        Collection unzip(const StringDict& zipped, bool inplace=true);
};

#endif /* ifndef COLLECTION_H */
