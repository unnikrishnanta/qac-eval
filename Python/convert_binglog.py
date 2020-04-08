#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Usage: python unicode_to_ascii uncode_file ascii_file
After the conversion, count the frequency of each string and export
the output as string    frequency 
Written by: Unni on 06/04/2020
"""


import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")

import argparse
from unidecode import unidecode
import sys
import csv
from collections import Counter
from collections import defaultdict
import re

maxInt = sys.maxsize
while True:
    # decrease the maxInt value by factor 10
    # as long as the OverflowError occurs.
    # https://stackoverflow.com/a/15063941/937153
    try:
        csv.field_size_limit(maxInt)
        break
    except OverflowError:
        maxInt = int(maxInt/10)


def convert_to_ascii(str_dict, verbose=False):
    """ Conver the strings in str_dict from utf-8 to ASCII.
    """
    ascii_list = list()
    non_converting = 0
    if verbose:
        print('\nNon converting strings:')
    for line in str_dict:
        ascii_str = unidecode(line[0])
        if(len(ascii_str)):
            ascii_list.append((ascii_str.strip(), line[1]))
        else:
            non_converting += 1
    print(non_converting, " conversations did not convert to ASCII")
    return ascii_list

def preprocess_str(s): 
    s = s.strip()
    s = s.lower()
    s = s.replace(u"–", "_")  # Replace all hyphens by whitespaces
    if not hasattr(preprocess_str, "pattern"): # Initialize it
        preprocess_str.pattern = re.compile('[\W_]+', re.UNICODE)
    filtered = preprocess_str.pattern.sub('_', s).strip("_")
    return filtered

def read_file(file_name):
    str_dict = list()
    empty_lines = 0
    with open(file_name, "r", encoding='utf-8') as infile:
        for line in infile.readlines():
            line = line.strip()
            if len(line) == 0: # strange!
                empty_lines += 1
            line = line.split('\t')
            print(line)
            s = preprocess_str(line[0])
            str_dict.append((s, int(line[1])))  # line -> partial queries
    return str_dict

def freq_accum(str_dict):
    freq_map = defaultdict(int)
    for line in str_dict: 
        freq_map[line[0]] += line[1]
    return freq_map

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert utf-8 log to ASCII')
    parser.add_argument('-v', action='store_true',
                        help='Run in verbose')
    parser.add_argument('-i','--infile', help='Input file in utf-8 format',
                        required=True)
    parser.add_argument('-o','--outfile', help='Output ASCII file',
                        required=False)
    args = vars(parser.parse_args())
    verbose = False
    if(args['v']):
        print('Running in verbose')
        verbose = True

    str_dict = read_file(args['infile'])
    print("File ", args['infile'], " loaded.")

    if args['outfile'] :
        export_file = args['outfile']
    else:
        export_file = "ascii-encoded.tsv"

    ascii_data = convert_to_ascii(str_dict, verbose=verbose)
    str_freq = freq_accum(ascii_data)
    # https://stackoverflow.com/a/3191811/937153
    with open(export_file, "w", encoding='ascii', newline='') as outfile:
        for k in str_freq:
            outfile.write('{0}:{1}\n'.format(k, str_freq[k]))

    print("Converted data written to ", export_file)

