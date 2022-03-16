import numpy as np
import matplotlib.pyplot as plt
import sys

import common_csv

def plot_channels(ch, rt, maxe, loglin, stacked):
    if stacked:
        fig, ax = plt.subplots(nrows=1, ncols=1)
        axs = np.array([ax] * 4)
    else:
        fig, axs = plt.subplots(nrows=2, ncols=2)

    binz = common_csv.make_bins(maxe, loglin)
    for ax, (chid, eng_ary) in zip(axs.ravel(), ch.items()):
        channel_dict = ch if stacked else {chid: eng_ary}
        common_csv.common_plot_cfg(fig, ax, loglin, rt, channel_dict)
        ax.hist(eng_ary, bins=binz, histtype='step', label=f'Channel {chid}')
        ax.legend(loc='best')

    fig.set_size_inches(12, 8)
    fig.tight_layout()
    plt.show()

def main():
    try:
        channelz, run_type, maxe, ll = common_csv.common_load()
        options = ('st', 'se')
        opt = ''
        while opt not in options:
            opt = input("stacked or separate histograms (st/se) ")
        plot_channels(channelz, run_type, maxe, ll, opt == 'st')
    except IndexError:
        print("usage: hist_geant_csv (input file)")
        exit(1)

if __name__ == '__main__': main()
