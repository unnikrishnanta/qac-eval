#include<iostream>
#include "core/collection.hpp"
#include "config.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    
    Collection coll_wiki;
    cout << "Reading wiki clickstream collection\n";
    coll_wiki.read_collection(WIKI_ROOT + "clickstream-agg-wiki.tsv");
    for(const auto sd: coll_wiki)
        cout << sd.first << "\t" << sd.second << "\n"; 
    return 0;
}
