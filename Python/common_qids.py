"""
Load all the partial query logs (Wiki, Cweb and Bing) and find the common qids
that are present in all three. Remove those qids from each log that are not
present in the common pool. Sort the logs based on qid, pick top n, print basic
stats, export sampled logs and the common qid list. 
"""

import pandas as pd
import csv 
import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import argparse
import numpy as np

def read_synthlog(file_name, nrows=None):
    if file_name.endswith('gz'):
        compression = 'gzip'
    else :
        compression = None
    
    synth_df = pd.read_csv(file_name, sep='\t', compression=compression,
                            names=['prefix_str', 'query_id'],
                            nrows=nrows, quoting=csv.QUOTE_NONE)
    synth_df.dropna(subset=['prefix_str'], inplace=True)
    synth_df['prefix_len'] = synth_df.prefix_str.str.len()
    return synth_df[~(synth_df.prefix_len == 0)]

def find_synth_end(synth_log):
    """ Return a dataframe containing only the last interaction from each 
    conversation in synth_log.
    """
    end_df = synth_log.index.to_series().groupby(
                            synth_log['query_id']).last().reset_index(name='end_idx')
    synth_end = synth_log[synth_log.index.isin(end_df.end_idx)]
    return synth_end

def load_synthlog(file_name, nrows=None):
    synth_log = read_synthlog(file_name, nrows)
    synth_end = find_synth_end(synth_log)
    return synth_log, synth_end

def print_basic_stats(log_df, log_end):
    print("Number of conversations: ", len(log_end))
    print("Number of interactions: ", len(log_df))
    # conv_lendf = conversation_lengths(log_df)
    # print("Median length of conversations: ", conv_lendf.conv_len.median())
    # print("Median length of partial queries ", log_df.prefix_len.median())
    print("Partial query lengths stats\n", log_df.prefix_len.describe())
    print("Unique partial queries: ", len(log_df.prefix_str.unique()))
    print("Unique final partial queries: ", len(log_end.prefix_str.unique()))

def find_common_qids(bing_end, wiki_end, cweb_end):
    common_df = wiki_end.merge(cweb_end, on='query_id', how='inner')\
                    .merge(bing_end, on='query_id', how='inner')
    common_df.dropna(inplace=True)
    common_qids = common_df.query_id.unique()
    return common_qids

def filter_and_sample(log_df, log_end, common_qids, n):
    qid_sample = np.random.choice(common_qids, n, replace=False)
    log_df = log_df[log_df.query_id.isin(qid_sample)]
    log_end = find_synth_end(log_df)
    return log_df, log_end

def export_logdf(log_df, file_name):
    file_stub = file_name.split('.tsv')
    export_file = file_stub[0] + '-sample.tsv'
    print("Export file: " , export_file)
    log_df[['prefix_str', 'query_id']].to_csv(export_file, sep='\t',
                                              header=None, index=None)

def main(args):
    nrows = None
    if args['testsize']: # Test mode
        nrows = int(args['testsize'])
        print('Running in test. Loading ', nrows, ' rows')

    sample_size = 100000
    if 'nconv' in args:
        sample_size = int(args['nconv'])
    elif args['testsize']: # Test mode
        sample_size = 10
    print('Sample size ', sample_size, ' conversations')

    print("Loading qac logs")
    bing_log, bing_end = load_synthlog(args['binglog'], nrows=nrows)
    wikisynth, wiki_end = load_synthlog(args['wikisynth'], nrows=nrows)
    cwebsynth, cweb_end = load_synthlog(args['cwebsynth'], nrows=nrows)

    # Find common qids and sample
    print("Finding common QIDs")
    common_qids = find_common_qids(bing_end, wiki_end, cweb_end)
    print("No. of common qids: " , len(common_qids))
    bing_log, bing_end = filter_and_sample(bing_log, bing_end, common_qids,
                                            sample_size)
    wikisynth, wiki_end = filter_and_sample(wikisynth, wiki_end, common_qids,
                                            sample_size)
    cwebsynth, cweb_end = filter_and_sample(cwebsynth, cweb_end, common_qids,
                                            sample_size)

    print("Basic BingLog stats")
    print("="*40)
    print_basic_stats(bing_log, bing_end)
    print("\n\nBasic WikiSynth stats")
    print("="*40)
    print_basic_stats(wikisynth, wiki_end)
    print("\n\nBasic CwebSynth stats")
    print("="*40)
    print_basic_stats(cwebsynth, cweb_end)

    # Export sampleds logs and common qids
    print("Exporting sampled logs")
    export_logdf(bing_log, args['binglog'])
    export_logdf(wikisynth, args['wikisynth'])
    export_logdf(cwebsynth, args['cwebsynth'])

    np.savetxt("common-qids.csv", common_qids, delimiter=",", fmt='%s')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare BingLog and SynthLog')
    parser.add_argument('-b','--binglog', help='Filename for loading BingLog',
                        required=True)
    parser.add_argument('-w','--wikisynth', help='Filename for loading WikiSynthLog',
                        required=True)
    parser.add_argument('-c','--cwebsynth', help='Filename for loading CwebSynthLog',
                        required=True)
    parser.add_argument('-t','--testsize', help='Run test mode loading -t\
                        <lines> ', required=False)
    parser.add_argument('-n','--nconv',
                        help='Number of conversations to sample from pq logs',
                        required=False)
    args = vars(parser.parse_args())
    main(args)
    print("Done")
