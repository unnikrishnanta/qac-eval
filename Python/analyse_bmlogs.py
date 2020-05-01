import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import argparse
from bmlog_processor import Collection, QACImpl, load_build_df
from plotter import MyPlt as mplt

def buildtime_collsize(combined_build):
    print("Plotting build time vs collection size")
    for qimpl in QACImpl.qac_impl_list():
        mplt.plot_buildtime_nrows(combined_build, qimpl, '{0}.pdf'.format(qimpl))

def buildtime_bytes_rate(combined_build):
    print("Plotting build time bytes rate")
    mplt.plot_barplot(combined_build, '{0}.pdf'.format('build-bytes-rate'))

def main(args):
    nrows = None
    if args['testsize']: # Test mode
        nrows = int(args['testsize'])
        print('Running in test. Loading ', nrows, ' rows')

    wiki_log = args['wikiclick']
    cweb_log = args['clueweb']
    bing_log = args['bingcoll']

    build_wiki = load_build_df(wiki_log, Collection.wiki)
    build_cweb = load_build_df(cweb_log, Collection.cweb)
    build_bing = load_build_df(bing_log, Collection.bing)
    combined_build = build_wiki.append(build_cweb, ignore_index=True)\
                               .append( build_bing, ignore_index=True)

    buildtime_collsize(combined_build)
    buildtime_bytes_rate(combined_build)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare BingLog and SynthLog')
    parser.add_argument('-bc','--bingcoll', help='Filename for loading BingLog',
                        required=True)
    parser.add_argument('-wc','--wikiclick', help='Filename for loading WikiSynthLog',
                        required=True)
    parser.add_argument('-cc','--clueweb', help='Filename for loading CwebSynthLog',
                        required=True)
    parser.add_argument('-t','--testsize', help='Run test mode loading -t\
                        <lines> ', required=False)
    args = vars(parser.parse_args())
    main(args)
