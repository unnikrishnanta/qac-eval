""" Conver the completion strings from BingLog from UTF-8 to ASCII. 
After the conversion, count the frequency of each string and export
the output as string    frequency 
Written by: Unni on 06/04/2020
"""

import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import csv 
import argparse


def preprocess_bing_logs(file_name, nrows=None, remove_empty=False):
    ''' Read the file into a dataframe and preprocess it.
    file_name: The input file, tab seperated.
    nrows: Number of lines to load from the log file 
    remove_empty: Remove interactions where the partial query is empty

    columns in the file:
        prefix      : Partial query seperated by whitespace. 
        timestamp   : interaction timestamp starting from 0.
        query_id    : A unique IF fo this convrsation.
        device      : 'd' for desktop and 'm' for mobile.
        date        : yyyy-mm-dd.
    '''
    dtype = {
        'prefix_str': object,
        'timestamp': np.int32,
        'query_id': np.int32,
        'device': str,
    }

    bing_logs = pd.read_csv(file_name, delimiter='\t', dtype=dtype,
                            parse_dates=['date'], infer_datetime_format=True,
                            nrows=nrows, quoting=csv.QUOTE_NONE)
    bing_logs.sort_values(['date', 'query_id', 'device', 'timestamp'],
                          inplace=True)
    bing_logs.reset_index(drop=True, inplace=True)  # Reset after sorting
    unique_id = bing_logs['date'].astype(str)\
                    + '_' + bing_logs['query_id'].astype(str)\
                    + '_' + bing_logs['device']
    bing_logs['query_id'] = unique_id

    if remove_empty:
        # Remove rows where prefix_str is NaN
        bing_logs.dropna(subset=['prefix_str'], inplace=True)
        bing_logs = bing_logs[bing_logs.prefix_str != ""]  # Remove if empty
    
    # Add two additional fields for later
    bing_logs.insert(1, 'prefix_len', bing_logs.prefix_str.str.len())
    bing_logs = bing_logs[['prefix_str', 'prefix_len', 'query_id']]
    return bing_logs



def main(args):
    print("Reading from ", args['infile'])
    print("Processing the file")
    processed = process_file(args['infile'], int(args['length']))
    print("Writing output to ", args['outfile'])
    export_to_file(processed, args['outfile'])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare BingLog and SynthLog')
    parser.add_argument('-i','--infile', help='Filename for loading the collection',
                        required=True)
    parser.add_argument('-o','--outfile', help='Export filename', required=True)
    args = vars(parser.parse_args())

    main(args)
    print("Done")


