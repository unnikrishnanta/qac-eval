# qac-eval
If the shared object is not found
export LD_LIBRARY_PATH=../src/qac_impl/marisa/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=../external/openssl/install/lib/:$LD_LIBRARY_PATH

## Install 
1. Google benchmark under external/ (Follow installation instructions give in
   the google behcmhark repo and download and set google test under external/behchmark/
2. Download and install openssl under external/openssl/install
   Create a directory install under QAC-Eval/external/openssl and use 
./config --prefix=<path>/QAC-Eval/external/openssl/install --openssldir=<path>/QAC-Eval/external/openssl/install '-Wl,-rpath,$(LIBRPATH)'
    make -j && make install

3. Download and install Marisa trie. Copy the files from the install directory
   to src/qac_impl/marisa


