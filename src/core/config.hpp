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
#define BING_NCONV 10  // TODO: Enter number of conversations. 

#define WIKI_NROWS 4560699
#define CWEB_NROWS 125000000  // TODO: Check if the file has been sampled
#define BING_NROWS 10   // TODO: Enter # lines in the processed Bing collection

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
#define SYNTHLOG ""  // TODO: Bing log (ascii) file name. 
#define PQLOG_NCONV BING_NCONV
#endif

#define NCOMP 8  // Default number of completions 

const std::string WIKI_ROOT = "../../AutoSuggestions/PyQAC/data/wiki/";
const std::string CWEB_ROOT = "../../AutoSuggestions/PyQAC/data/"
                            "Clue-500M-Anchor-Log-External/";
const std::string BING_ROOT = "";  // TODO : Dir path containing Bing collection
const std::string WIKI_COLLECTION = "clickstream-agg-wiki-32B.tsv";
const std::string CWEB_COLLECTION = "clueweb-agg-32B-sample.tsv";
const std::string BING_COLLECTION = "";  // TODO: Bing collection file name

#endif /* ifndef CONFIG_H */
