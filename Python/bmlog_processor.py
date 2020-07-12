import pandas as pd

class Collection:
    wiki = 'wiki'
    cweb = 'cweb'
    bing = 'bing'

class QACImpl : 
    htrie = 'HTrie'
    marisa = 'Marisa'
    dawg = 'DAWG'
    incngt = 'IncNgTrie'

    @staticmethod
    def qac_impl_list():
        return [QACImpl.htrie, QACImpl.marisa, QACImpl.dawg, QACImpl.incngt]

class Benchmark : 
    "Names of benchmarks"
    build = 'Build'
    query = 'Query'
    query_plen = 'QueryPlen'

    LRLog = 'LRLog'
    SynthLog = 'SynthLog'

def convert_time(log_df):
    if len(log_df.time_unit.unique()) > 1:
            raise ValueError('Mixed time units')
    time_unit = log_df.time_unit.unique()[0]
    if time_unit == 'ms':
        log_df.cpu_time = log_df.cpu_time/(10**3)  # ms to s
        log_df.real_time = log_df.real_time/(10**3)
    elif time_unit == 'ns':
        log_df.cpu_time = log_df.cpu_time/(10**9)  # ns to s
        log_df.real_time = log_df.real_time/(10**9)
    else:
        raise ValueError('Unrecognised time unit')

def load_build_df(filename, collection, convert_units=True):
    build_df = pd.read_csv(filename, index_col=None)
    build_df.drop(['bytes_per_second', 'items_per_second',
                 'label', 'error_occurred', 'error_message' ], axis=1, inplace=True)
    build_df['collection'] = collection
    if convert_units:
        convert_time(build_df)
    return build_df

def load_qtime_collsize_df(filename, collection, convert_units=True):
    """ Load query time vs collection size benchmark files."""
    try:
        query_df = pd.read_csv(filename)
    except (OSError, IOError) :
        print('File {0} not found!'.format(filename))
        return None
    name_split = query_df.name.str.split('/')
    qac_impl = name_split.apply(lambda x: x[1])
    coll_size = name_split.apply(lambda x: int(x[2]))
    pqlog_nconv = name_split.apply(lambda x: int(x[3]))
    log_type = name_split.apply(lambda x: x[4])
    log_type.replace('108', Benchmark.LRLog, inplace=True)
    log_type.replace('115', Benchmark.SynthLog, inplace=True)
    
    query_df['qac_impl'] = qac_impl
    query_df['nrows'] = coll_size
    query_df['nconv'] = pqlog_nconv  # Number of conversations used
    query_df['log_type'] = log_type
    query_df['collection'] = collection
    
    query_df.drop(['name','bytes_per_second','items_per_second',
                   'label','error_occurred','error_message'], axis=1, inplace=True)
    
    if convert_units:
        convert_time(query_df)
    
    return query_df

def map_qac_impl(s):
    for qimpl in QACImpl.qac_impl_list():
        if qimpl in s: 
            return qimpl

def load_qtime_df(filename, collection, convert_units=True):
    """Total querying time for each collection across Synth and LR logs. """
    query_df = pd.read_csv(filename, index_col=None)
    name_split = query_df.name.str.split('/')
    
    qac_impl = name_split.apply(lambda x: x[1])
    coll_size = name_split.apply(lambda x: int(x[2]))
    pqlog_nconv = name_split.apply(lambda x: int(x[3]))
    log_type = name_split.apply(lambda x: x[4])
    log_type.replace('108', 'LRLog', inplace=True)
    log_type.replace('115', 'SynthLog', inplace=True)
    
    query_df['qac_impl'] = qac_impl
    query_df['nrows'] = coll_size
    query_df['nconv'] = pqlog_nconv
    query_df['log_type'] = log_type
    query_df['collection'] = collection
    
    query_df.drop(['name','bytes_per_second','items_per_second',
                   'label','error_occurred','error_message'],
                   axis=1, inplace=True)
    
    query_df.qac_impl = query_df.qac_impl.apply(lambda x: map_qac_impl(x))
    if convert_units:
        convert_time(query_df)
    
    return query_df

def load_qtime_plen_df(file_name):
    """ Querying time at different values of |P|. """
    qtime_plen_df = pd.read_csv(file_name, header=None)
    qtime_plen_df.columns = ['qac_impl', 'plen', 'cpu_time',
                              'log_type', 'collection']
    qtime_plen_df.cpu_time = qtime_plen_df.cpu_time * 1e3 # s to ms
    qtime_plen_df.log_type = qtime_plen_df.log_type.map({115:'SynthLog', 108:'LRLog'})
    qtime_plen_df.qac_impl = qtime_plen_df.qac_impl.apply(lambda x: map_qac_impl(x))
    for logt in ['SynthLog', 'LRLog']:
        mask = (qtime_plen_df.log_type==logt)
        qtime_plen_df.loc[mask,
                'normed_cpu_time'] = qtime_plen_df[mask]['cpu_time'].div(
                                        qtime_plen_df[mask]['cpu_time'].sum())
    
    return qtime_plen_df

def read_membm_df(file_name, collection):
    """ Read memory benchmark log from file_name."""
    mem_df = pd.read_csv(file_name)
    mem_df['collection'] = collection
    mem_df.sort_values(['QACImpl','nrows'], inplace=True)
    mem_df.QACImpl = mem_df.QACImpl.apply(lambda x: map_qac_impl(x))
    mem_df.rename(columns={'QACImpl':'qac_impl'}, inplace=True)
    mem_df['heap_frac'] = mem_df.heap_used/mem_df.BytesProcessed
    return mem_df
