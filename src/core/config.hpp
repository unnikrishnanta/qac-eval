#ifndef CONFIG_H
#define CONFIG_H

#include<string>

#define WIKI 'w'
#define CWEB 'c'
#define BING 'b'

#define COLLECTION WIKI

#define SLOG 's'
#define LRLOG 'l'

#define WIKI_NCONV 1206653
#define CWEB_NCONV 1264108
#define BING_NCONV 1968265  

#define WIKI_NROWS 4560699
#define CWEB_NROWS 125000000  
#define BING_NROWS 11578960   

#define NCOMP 8  // Default number of completions 
#define MAX_STRLEN 32  // Max length of strings indexed

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__)
    #if COLLECTION==WIKI 
    #define NROWS WIKI_NROWS
    #define SYNTHLOG "../../synth_log/data/wiki-synthlog.tsv"
    #define PQLOG_NCONV WIKI_NCONV
    #elif COLLECTION==CWEB 
    #define NROWS CWEB_NROWS
    #define SYNTHLOG "../../synth_log/data/cweb-synthlog.tsv"
    #define PQLOG_NCONV CWEB_NCONV
    #else
    #define NROWS BING_NROWS
    #define SYNTHLOG "../../final_bing_log_ascii.tsv" 
    #define PQLOG_NCONV BING_NCONV
    #endif
    const std::string WIKI_ROOT = "../../AutoSuggestions/PyQAC/data/wiki/";
    const std::string CWEB_ROOT = "../../AutoSuggestions/PyQAC/data/Clue-500M-Anchor-Log-External/";
    const std::string BING_ROOT = "";  
    const std::string WIKI_COLLECTION = "clickstream-agg-wiki-32B.tsv";
    const std::string CWEB_COLLECTION = "clueweb-agg-32B-sample.tsv";
    const std::string BING_COLLECTION = "";
#endif  // ifdef unix

#if defined(_WIN32)
    #if COLLECTION==WIKI 
    #define NROWS WIKI_NROWS
    #define SYNTHLOG "../../synth_log/data/wiki-synthlog.tsv" //TODO:Update
    #define PQLOG_NCONV WIKI_NCONV
    #elif COLLECTION==CWEB 
    #define NROWS CWEB_NROWS
    #define SYNTHLOG "../../synth_log/data/cweb-synthlog.tsv" //TODO:Update
    #define PQLOG_NCONV CWEB_NCONV
    #else
    #define NROWS BING_NROWS
    #define SYNTHLOG "../../final_bing_log_ascii.tsv" //TODO:Update
    #define PQLOG_NCONV BING_NCONV
    #endif
    const std::string WIKI_ROOT = "../../AutoSuggestions/PyQAC/data/wiki/";
    const std::string CWEB_ROOT = "../../AutoSuggestions/PyQAC/data/Clue-500M-Anchor-Log-External/";
    const std::string BING_ROOT = "../../";
    const std::string WIKI_COLLECTION = "clickstream-agg-wiki-32B.tsv";
    const std::string CWEB_COLLECTION = "clueweb-agg-32B-sample.tsv";
    const std::string BING_COLLECTION = "completionFrequencies_processed.tsv";
#endif  // if defined (_WIN32)

#endif /* ifndef CONFIG_H */
