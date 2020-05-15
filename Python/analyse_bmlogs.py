import sys
if sys.version_info[0] < 3:
    raise Exception("Must be using Python 3")
import argparse
from bmlog_processor import Collection, QACImpl, Benchmark,\
                            load_build_df, load_qtime_collsize_df,\
                            load_qtime_plen_df
from plotter import MyPlt as mplt

def cputime_collsize(combined_df, benchmark, ylabel, style=None):
    for qimpl in QACImpl.qac_impl_list():
        mplt.plot_cputime_nrows(combined_df,
                                qac_impl=qimpl,
                                outfile='{0}-{1}.pdf'.format(benchmark, qimpl),
                                style=style,
                                ylabel=ylabel)

def buildtime_bytes_rate(combined_df):
    mplt.plot_barplot(combined_df, '{0}.pdf'.format('build-bytes-rate'))


def buildtime_plots(bm_root_dir):
    build_base = bm_root_dir + '/build/'
    build_wiki = load_build_df(build_base+'{0}.csv'.format(Collection.wiki),
                               Collection.wiki)
    build_cweb = load_build_df(build_base+'{0}.csv'.format(Collection.cweb),
                               Collection.cweb)
    build_bing = load_build_df(build_base+'{0}.csv'.format(Collection.bing),
                               Collection.bing)
    combined_build = build_wiki.append(build_cweb, ignore_index=True)\
                               .append( build_bing, ignore_index=True)

    print("Plotting build time vs collection size for collections")
    cputime_collsize(combined_build, benchmark=Benchmark.build,
                     ylabel="Build time")
    print("Plotting build time bytes rate")
    buildtime_bytes_rate(combined_build)


def query_time_bytes_rate(combined_qcs, cutoff_nrows, normalise=True):
    sliced_df = combined_qcs[(combined_qcs.log_type=="SynthLog")
                        & (combined_qcs.nrows <= cutoff_nrows)].copy()
    if normalise:
        sliced_df.PQBytesRate = sliced_df.PQBytesRate/sliced_df.nrows
    mplt.plot_boxplot(sliced_df, "qtime-pqbytes-rate.pdf",
                      ylabel="Bytes/sec")


def querytime_plots(bm_root_dir):
    qc_base = bm_root_dir + '/query-collsize/'
    wiki_qcs = load_qtime_collsize_df(qc_base+'{0}.csv'.format(Collection.wiki),
                                      Collection.wiki)
    cweb_qcs = load_qtime_collsize_df(qc_base+'{0}.csv'.format(Collection.cweb),
                                      Collection.cweb)
    bing_qcs = load_qtime_collsize_df(qc_base+'{0}.csv'.format(Collection.bing),
                                      Collection.bing)
    combined_qcs = wiki_qcs.append(cweb_qcs, ignore_index=True)\
                               .append(bing_qcs, ignore_index=True)
    print("Total query bytes rate")
    query_time_bytes_rate(combined_qcs, bing_qcs.nrows.max())
    print("Plotting query time vs collection size (SynthLog)")
    # combined_qcs[combined_qcs.log_type==Benchmark.SynthLog]
    cputime_collsize(combined_qcs,
                     benchmark=Benchmark.query,
                     ylabel="Querying time",
                     style='log_type')

    # Query time |P| benchmarks
    qplen_base = bm_root_dir + '/query-plen/'
    wiki_plenq_df = load_qtime_plen_df(qplen_base + '{0}.csv'.format(Collection.wiki),
                                       Collection.wiki)
    bing_plenq_df = load_qtime_plen_df(qplen_base + '{0}.csv'.format(Collection.bing),
                                       Collection.bing)
    combined_plenq_df = wiki_plenq_df.append(bing_plenq_df, ignore_index=True)
    benchmark = 'qplen'
    for qac_impl in QACImpl.qac_impl_list():
        mplt.plot_qtime_plen(combined_plenq_df, combined_qcs, qac_impl, 
                             outfile='{0}-{1}.pdf'.format(benchmark, qac_impl), 
                             style='log_type')
        
def main(args):
    nrows = None
    if args['testsize']: # Test mode
        nrows = int(args['testsize'])
        print('Running in test. Loading ', nrows, ' rows')

    bm_root_dir = args['dir']
    buildtime_plots(bm_root_dir)
    querytime_plots(bm_root_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare BingLog and SynthLog')
    parser.add_argument('-d','--dir', help='Location of export/data/time-bm',
                        required=True)
    parser.add_argument('-t','--testsize', help='Run test mode loading -t\
                        <lines> ', required=False)
    args = vars(parser.parse_args())
    main(args)
