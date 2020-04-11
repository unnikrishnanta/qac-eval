/* Randomly shuffle the collection to the given size. To obtain consistent
 * shuffling across multiple runs, the sampled indexes are stored in a file. 
 * The indexes can be loaded from this file to perform the sampling later. 
    g++ -std=c++17 sample-collection.cpp -o sample-collection
 */
#include <iostream>
#include <algorithm>
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <stdexcept>
#include <cassert>
#include <numeric>
#include <random>
#include "myprog_opts.hpp"
#include <sstream>
#include <fstream>

using namespace std;

/* Generate a vector containing 0... max_size. Shuffle this vector and return a
 * sub vector of size sample_size
 */
vector<size_t> sample_indexes(const size_t& sample_size, const size_t& max_size){
  assert(sample_size <= max_size);
  std::srand (unsigned ( std::time(0) ) );
  std::vector<size_t> index_vec(max_size);
  // Fill with 0, 1, ..., max_size-1
  std::iota (std::begin(index_vec), std::end(index_vec), 0); 
  // using built-in random generator:
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle (index_vec.begin(), index_vec.end(), g);
  return vector<size_t>(index_vec.begin(), index_vec.begin() + sample_size);
}

template <typename T>
void vec_to_file(const vector<T>& data, const string file_name){
    ofstream myfile;
    myfile.open (file_name);
    for (const auto& i : data) {
       myfile << i << "\n"; 
    }
    myfile.close();
}

template<typename T>
T getline_as(const string& s)
{
    std::stringstream convert(s);
    T value;
    convert >> value;
    return value;
}

vector<string> sample_collecton(const vector<string>& coll,
        const vector<size_t>& idx){
    assert(idx.size() <= coll.size());
    vector<string> sampled; 
    for (const auto& i: idx) {
        cout << "i = " << i << "\n";
       assert(i < coll.size());
       sampled.push_back(coll[i]);
    }
    return sampled;
}

template <typename T>
void load_file(vector<T>& ret_vec, const string& file_name,
        const string type="str") {
    vector<T> data;
    ifstream infile(file_name);
    if (infile) {  // test file open   
        std::string line;
        while (std::getline(infile, line)) {
            data.push_back(getline_as<T>(line));
        }
    }
    else {
        std::cerr << "To generate index file (if missing), run with -e.\n";
        throw std::runtime_error("Error opening " + file_name );
    }
    ret_vec.swap(data);
}

void load_collection(vector<string>& ret_vec, const string& file_name) {
    vector<string> data;
    ifstream infile(file_name);
    if (infile) {  // test file open   
        std::string line;
        while (std::getline(infile, line)) {
            data.push_back(line);
        }
    }
    else {
        std::cerr << "To generate index file (if missing), run with -e.\n";
        throw std::runtime_error("Error opening " + file_name );
    }
    ret_vec.swap(data);
}

int main(int argc, char *argv[])
{
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        std::cout << "Program options\n";
        std::cout << "==========================================================\n";
        std::cout << "-h Print usage.\n";
        std::cout << "-e Export sampled indexes\n";
        std:: cout << "-l file_name. Load sampled indexes from file\n";
        std::cout << "-s Sample size. Default: 128M\n";
        std::cout << "-i Input file_name\n";
        std::cout << "-o Output file_name\n";
        return 0;
    }

    bool sample = false;
    if (cmdOptionExists(argv, argv + argc, "-e"))
        sample = true;
    string index_file = "cweb-index.txt";
    char *idxf = getCmdOption(argv, argv + argc, "-l");
    if(idxf){
        index_file = std::string(idxf);
        std::cout  << "Loading indexes from: " << index_file << "\n";
    }
    string infile;
    char *in_f = getCmdOption(argv, argv + argc, "-i");
    if(in_f){
        infile = std::string(in_f);
        std::cout  << "Loading collection from " << infile << "\n";
    }
    else
        throw std::runtime_error("Provide collection with -i <file_name>");
    string outfile = "cweb-sample.tsv";
    char *o_f = getCmdOption(argv, argv + argc, "-o");
    if(o_f){
        outfile = std::string(o_f);
        std::cout  << "Output file: " << outfile << "\n";
    }
    size_t sample_size = 128000000;
    char *ss = getCmdOption(argv, argv + argc, "-s");
    if(ss){
        sample_size = std::atoi(ss);
    }
    std::cout  << "Sample size: " << sample_size << "\n";

    std::cout << "Reading collection\n";
    vector<string> coll;
    load_collection(coll, infile);
    auto coll_size = coll.size();
    std::cout << "Collection size " << coll_size << "\n";
    
    if(sample) {
        auto idx_sample = sample_indexes(sample_size, coll_size);
        vec_to_file(idx_sample, "cweb-index.txt");
        return 0;
    }

    vector<size_t> idx; 
    load_file(idx, index_file);
    cout << "Index file: \n";
    for (const auto& i : idx) {
       cout << i << "\n"; 
    }
    auto sampled = sample_collecton(coll, idx);
    vec_to_file(sampled, outfile);

    return 0;
}
