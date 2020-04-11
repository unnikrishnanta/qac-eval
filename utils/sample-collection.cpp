/* Randomly shuffle the collection to the given size. To obtain consistent
 * shuffling across multiple runs, the sampled indexes are stored in a file. 
 * The indexes can be loaded from this file to perform the sampling later. 
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
#include "../include/myprog_opts.hpp"
#include <fstream>

using namespace std;

/* Generate a vector containing 0... max_size. Shuffle this vector and return a
 * sub vector of size sample_size
 */
vector<size_t> sample_indexes(const size_t& sample_size, const size_t& max_size){
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
void index_to_file(const vector<T>& data, const string file_name){
    ofstream myfile;
    myfile.open (file_name);
    for (const auto& i : data) {
       myfile << i << "\n"; 
    }
    myfile.close();
}

vector<string> sample_collecton(const vector<string>& coll, const vector<size_t>& idx){
    assert(idx.size() <= coll.size());
    vector<string> sampled; 
    for (const auto& i: idx) {
       sampled.push_back(coll[i]);
    }
    return sampled;
}


template <typename T>
vector<T> load_file(const string& file_name) {
    vector<T> data;
    ifstream infile(file_name);
    if (inputFile) {  // test file open   
        T value;
        while ( infile >> value ) {
            data.push_back(value);
        }
    }
    else 
        throw std::runtime_error("Error opening " + file_name );
    
    return data;
}

int main(int argc, char *argv[])
{
    if (cmdOptionExists(argv, argv + argc, "-h")) {
        std::cout << "Program options\n";
        std::cout << "==========================================================\n";
        std::cout << "-h Print usage.";
        std::cout << "-e Export sampled indexes\n";
        std:: cout << "-l file_name. Load sampled indexes from file\n";
        std::cout << "-s Sample size.\n";
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
        index_file = std::string(idxg);
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
    vector<string> coll = load_file(infile);
    auto coll_size = coll.size();
    std::cout << "Collection size " << coll_size << "\n";
    if(sample) {
        auto idx_sample = sample_indexes(sample_size, coll_size);
        index_to_file("cweb-index.txt");
        return 0;
    }

    vector<size_t> idx = load_file(index_file);
    auto sampled = sample_collecton(coll, idx);
    


    return 0;
}
