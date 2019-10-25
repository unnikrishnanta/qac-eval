#include<iostream>
#include "core/collection.hpp"
#include "config.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    
    Collection coll_wiki;
    coll_wiki.read_collection(WIKI_ROOT + "clickstream-agg-wiki.tsv");
    return 0;
}
