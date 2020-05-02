#ifndef CONFIG_H
#define CONFIG_H

#include<string>
#include <stdio.h>

// Collection names
#define WIKI 'w'
#define CWEB 'c'
#define BING 'b'

//Current collection being benchmarked
#define COLLECTION CWEB

// Log type. SLOG: Bing/Synth logs. LRLOG: Left to Right Log
#define SLOG 's'
#define LRLOG 'l'

#define NCOMP 8  // Default number of completions 
#define MAX_STRLEN 32  // Max length of strings indexed

// Number of conversations (not #rows) from each partial query log
/* #define WIKI_NCONV 1206653 */
/* #define CWEB_NCONV 1264108 */
/* #define BING_NCONV 1968265 */  
#define WIKI_NCONV 100000
#define CWEB_NCONV 100000
#define BING_NCONV 100000  

// Number of rows from each collection
#define WIKI_NROWS 4560699
#define CWEB_NROWS 125000000  
#define BING_NROWS 11578961   

#define WINDOWS "Windows"
#define LINUX "Linux" 
#define APPLE "Apple"
#define UNKNOWN "Unknown"

#if defined(__linux__)
    #define PLATFORM LINUX
#elif defined(__APPLE__)
    #define PLATFORM APPLE
#elif defined(_WIN32) || defined(__CYGWIN__)
    #define PLATFORM WINDOWS
#else
    #define PLATFORM UNKNOWN
#endif

// Keeping the below old code as such without using the macro PLATFORM
#if defined(__linux__) || defined(__APPLE__)
    const std::string WIKI_ROOT = "../../AutoSuggestions/PyQAC/data/wiki/";
    const std::string CWEB_ROOT = "../../AutoSuggestions/PyQAC/data/Clue-500M-Anchor-Log-External/";
    const std::string BING_ROOT = "";  
    const std::string WIKI_COLLECTION = "clickstream-agg-wiki-32B.tsv";
    const std::string CWEB_COLLECTION = "clueweb-agg-32B-sample.tsv";
    const std::string BING_COLLECTION = "";
    #if COLLECTION==WIKI 
        #define NROWS WIKI_NROWS
        #define SYNTHLOG "../../synth_log/data/wiki-synthlog-sample.tsv"
        #define PQLOG_NCONV WIKI_NCONV
    #elif COLLECTION==CWEB 
        #define NROWS CWEB_NROWS
        #define SYNTHLOG "../../synth_log/data/cweb-synthlog-sample.tsv"
        #define PQLOG_NCONV CWEB_NCONV
    #else
        #define NROWS BING_NROWS
        #define SYNTHLOG "../../final_bing_log_ascii-sample.tsv" 
        #define PQLOG_NCONV BING_NCONV
    #endif

#elif defined(_WIN32) || defined(__CYGWIN__)
    const std::string WIKI_ROOT = "../../";
    const std::string CWEB_ROOT = "../../";
    const std::string BING_ROOT = "../../";
    const std::string WIKI_COLLECTION = "clickstream-agg-wiki-32B.tsv";
    const std::string CWEB_COLLECTION = "clueweb-agg-32B-sample.tsv";
    const std::string BING_COLLECTION = "completionFrequencies_processed.tsv";
    #if COLLECTION==WIKI 
        #define NROWS WIKI_NROWS
        #define SYNTHLOG "../../wiki-synthlog-sample.tsv"
        #define PQLOG_NCONV WIKI_NCONV
    #elif COLLECTION==CWEB 
        #define NROWS CWEB_NROWS
        #define SYNTHLOG "../../cweb-synthlog-sample.tsv"
        #define PQLOG_NCONV CWEB_NCONV
    #else
        #define NROWS BING_NROWS
        #define SYNTHLOG "../../final_bing_log_ascii-sample.tsv"
        #define PQLOG_NCONV BING_NCONV
    #endif
#endif  // if defined (unix)

#endif /* ifndef CONFIG_H */

