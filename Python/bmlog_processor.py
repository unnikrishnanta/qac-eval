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
    build_df = pd.read_csv(filename, index_col=None)
    build_df.drop(['bytes_per_second', 'items_per_second',
                 'label', 'error_occurred', 'error_message' ], axis=1, inplace=True)
    build_df['collection'] = collection
    if convert_units:
        build_df.cpu_time = build_df.cpu_time/(10**9)  # ns to s
        build_df.real_time = build_df.real_time/(10**9)
        build_df.TotalBytes =  build_df.TotalBytes/1024/1024 # B to MB
        build_df.TotalBytesRate = build_df.TotalBytesRate/1024/1024 # B to MB
    return build_df

