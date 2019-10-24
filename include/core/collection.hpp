#include<string>
#include<vector>
#include<utility>

using namespace std;

class Collection {
    using sdict_t = vector<pair<string, size_t>>;

    public:
        using iterator = sdict_t::iterator;
        using const_iterator = sdict_t::const_iterator;

        void read_collection(const string& file_name);
        const_iterator begin() const { return str_dict.begin(); }
        const_iterator end() const { return str_dict.end(); }

    private:
        sdict_t str_dict;
        
};

