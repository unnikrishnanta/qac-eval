import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from bmlog_processor import Benchmark, QACImpl
import pandas as pd

plt.switch_backend('agg')

class MyPlt:
    """ Custom class encapsulating various plotting functions.
    """
    BING_LOG_LABEL = r'{\tt Bing-QAC-2018}'
    WIKI_LOG_LABEL = r'{\tt Wiki-Synth}'
    CWEB_LOG_LABEL = r'{\tt CWeb-Synth}'
    BING_COLL_LABEL = r'{\tt Bing-Collection}'
    WIKI_COLL_LABEL = r'{\tt Wiki-Clickstream}'
    CWEB_COLL_LABEL = r'{\tt ClueWeb-09}'
    HTRIE_LABEL = r'HAT-trie'
    MARISA_LABEL = r'MARISA-trie'
    DAWG_LABEL = r'DAWG-trie'
    INCNGT_LABEL = r'IncNgTrie'
    QAC_IMPL_LABELS = {
                        QACImpl.htrie : HTRIE_LABEL,
                        QACImpl.marisa: MARISA_LABEL,
                        QACImpl.dawg  : DAWG_LABEL,
                        QACImpl.incngt: INCNGT_LABEL
                        }

    TEX_LASTP = r'($\mbox{\sf{FinalP}}$)'
    BING_LP_LABEL = BING_LOG_LABEL + TEX_LASTP
    WIKI_LP_LABEL = WIKI_LOG_LABEL + TEX_LASTP
    CWEB_LP_LABEL = CWEB_LOG_LABEL + TEX_LASTP

    WIKI_COLOR = '#486824'
    CWEB_COLOR = '#F2C057'
    BING_COLOR = '#D13525'
    PALETTE_ = [WIKI_COLOR, CWEB_COLOR, BING_COLOR]
    # Old colors
    COLOR2 = '#00a5c6'
    COLOR1 = '#b2d800'
    COLOR4 = '#fb6542'
    COLOR3 = '#ffbb00'
    _DIR_PATH = '../export/images/'
    _TEST_DIR_PATH = '../test/images/'
    _AX_LABEL_SIZE = 36
    _TICK_LABEL_SIZE = 28
    _LEGEND_LABEL_SIZE = 26
    _LINE_WIDTH = 6
    _DASHL_WIDTH = _LINE_WIDTH
    _MARKER_SIZE = _LINE_WIDTH + 10
    _dev_run = False

    @staticmethod
    def _initialise_plot():
        sns.set(rc={
                        'figure.figsize': (11.7, 8.27),  # Standard A4
                        # 'figure.figsize': (12.2, 7.37),  # ECIR dimensions
                        'text.usetex' : True
                    },
                style="white",
                color_codes=True)
        plt.rc('pdf', fonttype=42) # Embed font

    @staticmethod
    def _save_and_clear(outfile, despine=True):
        plt.tight_layout()
        if despine:
            sns.despine()
        if MyPlt._dev_run:
            plt.savefig(MyPlt._TEST_DIR_PATH + outfile, dpi=100)
        else:
            plt.savefig(MyPlt._DIR_PATH + outfile, dpi=100)

        plt.clf()  # Clear the current plot

    def plot_cputime_nrows(combined_df, qac_impl, outfile,
                           cutoff_nrows=None, style=None,
                           drop_maxrows = True,
                           xlabel="Collection size", ylabel="Build time"):
        """ Plot the relation between the collection size and cpu time for the
            given qac_impl for all the collections in combined_df.

            qac_impl: One of the values from class QACImpl
            style : Seaborn lineplot style argument. 
        """
        MyPlt._initialise_plot()
        qac_impl = qac_impl  # To get "BuildHtrie" for instanc
        if cutoff_nrows == None: # Set as  min nrows for any of the collection 
            # cutoff_nrows = combined_df.groupby(
            #                 combined_df.collection)['nrows'].agg(max).min()
            # cutoff_nrows = 2**21  # Second largest value for wikiclick
            cutoff_nrows = float('inf')
        slice_df = combined_df[(combined_df.qac_impl == qac_impl)
                                   & (combined_df.nrows <= cutoff_nrows)]

        if drop_maxrows:
            if 'log_type' in slice_df.columns:
                max_idx = slice_df.groupby(['collection', 'log_type'])['nrows'].idxmax()
            else:
                max_idx = slice_df.groupby(['collection'])['nrows'].idxmax()
            slice_df = slice_df.drop(max_idx, axis=0)

        sns.set_palette(MyPlt.PALETTE_)
        ax = sns.lineplot(x="nrows", y="cpu_time", hue="collection",
                          data=slice_df,style=style, marker="o",
                          alpha=.6)

        ax.set_xscale('log', basex=2)
        ax.set_yscale('log', basey=2)

        ymax = np.log2(combined_df[combined_df.nrows
                                      <= cutoff_nrows]['cpu_time'].max())
        ymin = np.log2(combined_df[combined_df.nrows
                                      <= cutoff_nrows]['cpu_time'].min())
        ax.set_ylim([2**(ymin-1), 2**(ymax+1)])
        
        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
        
        plt.setp(ax.lines,linewidth=MyPlt._LINE_WIDTH)  # set lw for all lines
        plt.setp(ax.lines,markersize=MyPlt._MARKER_SIZE)  # set lw for all lines
        
        # Legend position and line width
        leg = plt.legend(loc='upper left', prop={'size': MyPlt._LEGEND_LABEL_SIZE})
        for legobj in leg.legendHandles:
            legobj.set_linewidth(MyPlt._LINE_WIDTH)
        # Legend texts
        leg.get_texts()[0].set_text('Collection')
        for t in leg.get_texts()[1:]:
            if t.get_text() == u'bing':
                t.set_text(MyPlt.BING_COLL_LABEL)
            elif t.get_text() == u'cweb':
                t.set_text(MyPlt.CWEB_COLL_LABEL)
            elif t.get_text() == u'wiki':
                t.set_text(MyPlt.WIKI_COLL_LABEL)
            elif t.get_text() == u'log_type':
                t.set_text('Log type')
            elif t.get_text() == Benchmark.SynthLog\
                    or t.get_text() == Benchmark.LRLog:
                pass
            else:
                raise ValueError('Invalid collection name: ' + str(t))
        MyPlt._save_and_clear(outfile)


    def plot_qtime_plen(combined_plenq_df, combined_qtime_csize,
                        qac_impl, outfile, style=None,
                        xlabel="Length of partial query",
                        ylabel="Ratio of querying time",
                        plot_legend=False):
        pd.options.mode.chained_assignment = None  # default='warn'

        sliced_df = combined_plenq_df[combined_plenq_df.qac_impl==qac_impl]
        
        ax = sns.lineplot(x='plen', y='normed_cpu_time',
                          data=sliced_df,
                          hue='collection', style=style,
                          marker='o', markevery=4, alpha=.6)

        # ax.set_xscale('log', basex=2)
        ax.set_yscale('log', basey=10)
        ax.set_xlim([None, 80])
        # ax.set_ylim([10**-7, 1])

        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
        
        plt.setp(ax.lines,linewidth=MyPlt._LINE_WIDTH-1)  # set lw for all lines
        plt.setp(ax.lines,markersize=MyPlt._MARKER_SIZE-1)  # set lw for all lines
        
        if plot_legend:
            # Legend position and line width
            leg = plt.legend(loc='upper right', prop={'size': MyPlt._LEGEND_LABEL_SIZE})
            for legobj in leg.legendHandles:
                legobj.set_linewidth(MyPlt._LINE_WIDTH)
            # Legend texts
            leg.get_texts()[0].set_text('Collection')
            for t in leg.get_texts()[1:]:
                if t.get_text() == u'bing':
                    t.set_text(MyPlt.BING_COLL_LABEL)
                elif t.get_text() == u'cweb':
                    t.set_text(MyPlt.CWEB_COLL_LABEL)
                elif t.get_text() == u'wiki':
                    t.set_text(MyPlt.WIKI_COLL_LABEL)
                elif t.get_text() == u'log_type':
                    t.set_text('Log type')
                elif t.get_text() == Benchmark.SynthLog\
                        or t.get_text() == Benchmark.LRLog:
                    pass
                else:
                    raise ValueError('Invalid collection name: ' + str(t))
        else:
            ax.legend_.remove()
        MyPlt._save_and_clear(outfile)
        pd.options.mode.chained_assignment = 'warn'

    @staticmethod
    def plot_barplot(combined_df, outfile, ylabel='Bytes/sec',
                     hue='collection', 
                     x = "qac_impl", y ="TotalBytesRate"):
        MyPlt._initialise_plot()
        palette = sns.color_palette(MyPlt.PALETTE_)
        
        ax = sns.barplot(x=x, y=y, hue=hue, data=combined_df,
                         palette=palette, estimator=np.mean)
        plt.yscale('log', basey=2)
        xlabel = "Implementation"
        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
         
        xtick_labels = [item.get_text() for item in ax.get_xticklabels()]
        for i, item in enumerate(xtick_labels):
            if item in MyPlt.QAC_IMPL_LABELS: 
                xtick_labels[i] = MyPlt.QAC_IMPL_LABELS[item]
            else:
                raise ValueError('Plotter: Label not recognised')
        ax.set_xticklabels(xtick_labels)

        leg = plt.legend(loc='upper right', prop={'size': MyPlt._LEGEND_LABEL_SIZE})
        for legobj in leg.legendHandles:
            legobj.set_linewidth(MyPlt._LINE_WIDTH)
        if hue == 'collection':
            for t in leg.get_texts():
                if t.get_text() == u'bing':
                    t.set_text(MyPlt.BING_COLL_LABEL)
                elif t.get_text() == u'cweb':
                    t.set_text(MyPlt.CWEB_COLL_LABEL)
                elif t.get_text() == u'wiki':
                    t.set_text(MyPlt.WIKI_COLL_LABEL)
                else:
                    raise ValueError('Invalid collection name: ' + str(t))    
        
        plt.tight_layout()
        MyPlt._save_and_clear(outfile, despine=True)


    @staticmethod
    def plot_boxplot(combined_df, outfile,
                     ylabel='Bytes/sec',
                     hue='collection', x = "qac_impl", y ="PQBytesRate"):
        MyPlt._initialise_plot()
        palette = sns.color_palette(MyPlt.PALETTE_)
        
        ax = sns.boxplot(x=x, y=y, data=combined_df,  hue=hue, palette=palette)
        plt.yscale('log', basey=2)
        xlabel = "Implementation"
        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
         
        xtick_labels = [item.get_text() for item in ax.get_xticklabels()]
        for i, item in enumerate(xtick_labels):
            if item in MyPlt.QAC_IMPL_LABELS: 
                xtick_labels[i] = MyPlt.QAC_IMPL_LABELS[item]
            else:
                raise ValueError('Plotter: Label not recognised')
        ax.set_xticklabels(xtick_labels)

        leg = plt.legend(loc='upper left',
                         ncol=3,
                         bbox_to_anchor=(-0.01, 1.09),
                         frameon=True,
                         prop={'size': MyPlt._LEGEND_LABEL_SIZE-2})
        for legobj in leg.legendHandles:
            legobj.set_linewidth(MyPlt._LINE_WIDTH)
        if hue == 'collection':
            for t in leg.get_texts():
                if t.get_text() == u'bing':
                    t.set_text(MyPlt.BING_LOG_LABEL)
                elif t.get_text() == u'cweb':
                    t.set_text(MyPlt.CWEB_LOG_LABEL)
                elif t.get_text() == u'wiki':
                    t.set_text(MyPlt.WIKI_LOG_LABEL)
                else:
                    raise ValueError('Invalid collection name: ' + str(t))    
        
        # plt.tight_layout()
        MyPlt._save_and_clear(outfile, despine=True)



