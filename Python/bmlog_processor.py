import pandas as pd

class Collection:
    wiki = 'wiki'
    cweb = 'cweb'
    bing = 'bing'

class QACImpl : 
    htrie_build = 'BuildHTrie'
    mstrie_build = 'BuildMarisa'
    dawg_build = 'BuildDAWG'
    incngt_build = 'BuildIncNgT'

    @staticmethod
    def qac_impl_list():
        return [QACImpl.htrie_build, QACImpl.mstrie_build,
                QACImpl.dawg_build, QACImpl.incngt_build]

def load_build_df(filename, collection, convert_units=True):
    """ Load build time benchmark logs for collection. """
    build_df = pd.read_csv(filename, index_col=None)
    build_df.drop(['bytes_per_second', 'items_per_second',
                 'label', 'error_occurred', 'error_message' ], axis=1, inplace=True)
    build_df['collection'] = collection
    if convert_units:
        if len(build_df.time_unit.unique()) > 1:
            raise ValueError('Mixed time units')
        time_unit = build_df.time_unit.unique()[0]
        if time_unit == 'ns':
            build_df.cpu_time = build_df.cpu_time/(10**9)  # ns to s
            build_df.real_time = build_df.real_time/(10**9)
        else:
            raise ValueError('Unrecognised time unit')
    return build_df

def load_qtime_collsize_df(collection, convert_units=True):
    """ Load querying time vs collection size benchmark logs. """
    query_df = pd.read_csv('../../export/data/query-times-{0}-collsize.csv'\
                           .format(collection))
    name_split = query_df.name.str.split('/')
    qac_impl = name_split.apply(lambda x: x[1])
    coll_size = name_split.apply(lambda x: x[2])
    pqlog_nconv = name_split.apply(lambda x: x[3])
    log_type = name_split.apply(lambda x: x[4])
    log_type.replace('108', 'LRLog', inplace=True)
    log_type.replace('115', 'SynthLog', inplace=True)
    
    query_df['qac_impl'] = qac_impl
    query_df['coll_size'] = coll_size
    query_df['nconv'] = pqlog_nconv
    query_df['log_type'] = log_type
    query_df['collection'] = collection
    
    query_df.drop(['name','bytes_per_second','items_per_second',
                   'label','error_occurred','error_message'],
                   axis=1, inplace=True)

    if convert_units:
        if len(query_df.time_unit.unique()) > 1:
            raise ValueError('Mixed time units')
        time_unit = query_df.time_unit.unique()[0]
        if time_unit == 'ms':
            query_df.cpu_time = query_df.cpu_time/(10**3)  # ns to s
            query_df.real_time = query_df.real_time/(10**3)
        else:
            raise ValueError('Unrecognised time unit')
        
    return query_df
