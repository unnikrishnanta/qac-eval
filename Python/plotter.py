import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

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
    _DASHL_WIDTH = 5
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

    def plot_buildtime_nrows(combined_build, qac_impl, outfile,
                             cutoff_nrows=None,
                             xlabel="Collection size", ylabel="Build time"):
        """ Plot the relation between the collection size and build time for the
        given qac_impl for all the collections in combined_build. """
        MyPlt._initialise_plot()
        if cutoff_nrows == None: # Set as  min nrows for any of the collection 
            cutoff_nrows = combined_build.groupby(
                            combined_build.collection)['nrows'].agg(max).min()
        slice_df = combined_build[(combined_build.name == qac_impl)
                                   & (combined_build.nrows <= cutoff_nrows)]

        sns.set_palette(MyPlt.PALETTE_)
        ax = sns.lineplot(x="nrows", y="cpu_time", hue="collection",
                          data=slice_df, marker="o", alpha=.6)
        ax.set_xscale('log', basex=2)
        ax.set_yscale('log', basey=2)
        ymax = np.log2(combined_build[combined_build.nrows
                                      <= cutoff_nrows]['cpu_time'].max())
        ymin = np.log2(combined_build[combined_build.nrows
                                      <= cutoff_nrows]['cpu_time'].min())
        ax.set_ylim([2**(ymin-1), 2**(ymax+1)])
        
        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        # ylabel = ylabel + '(s)'
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
            else:
                raise ValueError('Invalid collection name: ' + str(t))
        MyPlt._save_and_clear(outfile)

    @staticmethod
    def plot_barplot(combined_build, outfile, ylabel='Bytes/sec',
                     data='collection'):
        MyPlt._initialise_plot()
        palette = sns.color_palette(MyPlt.PALETTE_)
        
        sns.barplot(x="name", y="TotalBytesRate", hue="collection", 
                    data=combined_build, palette=palette, estimator=np.mean)
        plt.yscale('log', basey=2)
        xlabel = "Implementation"
        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
         
        leg = plt.legend(loc='upper right', prop={'size': MyPlt._LEGEND_LABEL_SIZE})
        for legobj in leg.legendHandles:
            legobj.set_linewidth(MyPlt._LINE_WIDTH)
        
        if data == 'collection':
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


