import matplotlib.pyplot as plt
import seaborn as sns
from collections import Counter
import numpy as np
import copy
from math import atan2,degrees
import matplotlib.ticker as ticker

plt.switch_backend('agg')

class MyPlt:
    """ Custom class encapsulating various plotting functions.
    """
    BING_LABEL = r'{\tt Bing-QAC-2018}'
    WIKI_LABEL = r'{\tt Wiki-Synth}'
    CWEB_LABEL = r'{\tt CWeb-Synth}'

    TEX_LASTP = r' ($\mbox{\sf{FinalP}}$)'
    BING_LP_LABEL = BING_LABEL + TEX_LASTP
    WIKI_LP_LABEL = WIKI_LABEL + TEX_LASTP
    CWEB_LP_LABEL = CWEB_LABEL + TEX_LASTP

    WIKI_COLOR = '#486824'
    CWEB_COLOR = '#F2C057'
    BING_COLOR = '#D13525'
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
                    #'figure.figsize': (11.7, 8.27),
                    'figure.figsize': (12.2, 7.37),
                    'text.usetex' : True },
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

    @staticmethod
    def normalise_dict(d):
        '''Normalise the values in the dict d'''
        d_cpy = copy.deepcopy(d)
        factor = 1.0/sum(list(d_cpy.values()))
        for k in d_cpy:
            d_cpy[k] = d_cpy[k]*factor
        return d_cpy

    @staticmethod
    def plot_buildtime_nrows(combined_build, impl, outfile, cutoff_nrows=None,
                                xlabel="Collection size", ylabel="Build time"):
        from matplotlib.ticker import MultipleLocator, FuncFormatter
        MyPlt._initialise_plot()
        if cutoff_nrows == None: 
            cutoff_nrows = combined_build.groupby(combined_build.collection)['nrows'].agg(max).min()
        slice_df = combined_build[(combined_build.name == impl)
                    & (combined_build.nrows <= cutoff_nrows)]
        ax = sns.lineplot(x="nrows", y="cpu_time", hue="collection", data=slice_df, 
                            marker="o", alpha=.6)
        ax.set_xscale('log', basex=2)
        
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
                t.set_text(MyPlt.BING_LABEL)
            elif t.get_text() == u'cweb':
                t.set_text(MyPlt.CWEB_LABEL)
            elif t.get_text() == u'wiki':
                t.set_text(MyPlt.WIKI_LABEL)
            else:
                raise ValueError('Invalid collection name: ' + str(t))
        MyPlt._save_and_clear(outfile)

def plot_paired_hist(distr1, distr2, dlabel1, dlabel2, xlabel,
                        outfile, nbins=40, density=True, scale='linear'):
        MyPlt._initialise_plot()

        plt.hist(distr1, density=density, bins=np.arange(nbins)-0.5, alpha=.5,
                label=dlabel1, color=MyPlt.BING_COLOR)
        plt.hist(distr2, density=density, bins=np.arange(nbins)-0.5, alpha=.5,
                label=dlabel2, color=MyPlt.COLOR2)

        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        if density:
            plt.ylabel("Probability density", fontsize=MyPlt._AX_LABEL_SIZE)
        else:
            plt.ylabel("Frequency", fontsize=MyPlt._AX_LABEL_SIZE)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
        plt.legend(loc='upper right', prop={'size': MyPlt._TICK_LABEL_SIZE})
        plt.yscale(scale)
        plt.xlim(0)
        MyPlt._save_and_clear(outfile)

    @staticmethod
    def plot_paired_lineplot(distr1, distr2, label1, label2,
                             outfile,
                             xlabel, ylabel,
                             normalise=False,
                             markevery=0.1,
                             logx=False, logy=False,
                             show_markers=False):
        MyPlt._initialise_plot()
        plt.plot(distr1[0], distr1[1],
                marker='o', linewidth=MyPlt._LINE_WIDTH,
                markersize=MyPlt._MARKER_SIZE,
                color=MyPlt.BING_COLOR, label=label1, alpha=.7,
                markevery=markevery)
        plt.plot(distr2[0], distr2[1],
                marker='s', linewidth=MyPlt._LINE_WIDTH,
                markersize=MyPlt._MARKER_SIZE,
                color=MyPlt.COLOR2, label=label2, alpha=.7,
                markevery=markevery)
        if logy:
            plt.yscale('log')
        if logx:
            plt.xscale('log')

        if not show_markers:
            ax = plt.gca()
            for line in ax.lines:
                line.set_marker(None)

        plt.xlabel(xlabel, fontsize=MyPlt._AX_LABEL_SIZE)
        plt.ylabel(ylabel, fontsize=MyPlt._AX_LABEL_SIZE)

        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.xticks(fontsize=MyPlt._TICK_LABEL_SIZE, rotation=0)
        plt.yticks(fontsize=MyPlt._TICK_LABEL_SIZE)
        plt.legend(loc='upper right', prop={'size': MyPlt._TICK_LABEL_SIZE})
        # plt.xlim(0)

        MyPlt._save_and_clear(outfile)

    def plot_paired_boxplot(df, xcol, ycol, hue, outfile, show_legend=True):
        MyPlt._initialise_plot()
        ax = sns.boxplot(x=xcol, y=ycol, hue=hue, data=df,
                palette=[MyPlt.WIKI_COLOR, MyPlt.CWEB_COLOR, MyPlt.BING_COLOR])
        ax.set_xlabel("Error category",fontsize=MyPlt._AX_LABEL_SIZE)
        ax.set_ylabel("Percent. word length",fontsize=MyPlt._AX_LABEL_SIZE)

        ax.tick_params(axis="x", labelsize=MyPlt._TICK_LABEL_SIZE-4)
        ax.tick_params(axis="y", labelsize=MyPlt._TICK_LABEL_SIZE)

        if show_legend:
            plt.legend(loc='upper right', prop={'size': MyPlt._TICK_LABEL_SIZE},
                    ncol=2, bbox_to_anchor= (1., 1.1))
        else:
            ax.legend_.remove()

        sns.despine(offset=10, trim=True)
        MyPlt._save_and_clear(outfile, despine=False)



