#include<string>
#include<vector>
#include<utility>

using namespace std;

using sdict_t = vector<pair<string, size_t>>;

class Collection {

    public:
        using iterator = sdict_t::iterator;
        using const_iterator = sdict_t::const_iterator;

        void read_collection(const string& file_name, 
                             const size_t& n_rows=SIZE_MAX,
                             const bool& skip_header=false);
        const_iterator begin() const { return str_dict.begin(); }
        const_iterator end() const { return str_dict.end(); }
        const sdict_t get_collection() const;
        void sort_keys();

    private:
        sdict_t str_dict;
        
};

