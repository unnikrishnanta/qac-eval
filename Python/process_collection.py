"""
This program reads a file of the format string  score and applies the following
set of pre-processing 
    1. Remove strings that are longer than the cut-off length 
"""
import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import csv 
import argparse

def apply_processing(s, cut_off_len=32):
    s = s.lower()
    if len(s) <= cut_off_len:
        return s
    return None

def process_file(file_name, cut_off_len):
    processed = list()
    with open(file_name) as infile: 
        csv_reader = csv.reader(infile, delimiter='\t')
        for row in csv_reader:
            s = apply_processing(row[0], cut_off_len)
            if(s):
                processed.append((s, row[2]))
    return processed


def export_to_file(data, file_name):
    with open(file_name, 'w') as outfile: 
        csv_writer  = csv.writer(outfile, delimiter='\t')
        for row in data: 
            csv_writer.writerow(list(row))

def main(args):
    print("Reading from ", args['infile'])
    print("Cut off length ", args['length'])
    print("Processing the file")
    processed = process_file(args['infile'], int(args['length']))
    print("Writing output to ", args['outfile'])
    export_to_file(processed, args['outfile'])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare BingLog and SynthLog')
    parser.add_argument('-i','--infile', help='Filename for loading the collection',
                        required=True)
    parser.add_argument('-o','--outfile', help='Export filename', required=True)
    parser.add_argument('-l','--length', help='Cut off string length', required=True)
    args = vars(parser.parse_args())

    main(args)
    print("Done")

