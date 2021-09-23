import numpy as np
import matplotlib.pyplot as plt
import sys

import common_csv

def make_bins(max_eng, log_lin):
    if log_lin == 'log':
        return np.logspace(0, np.log10(max_eng * 1.2), num=1000)
    return np.linspace(1, max_eng * 1.2, num=1000)

def plot_channels(ch, rt, maxe, loglin):
    fig, ax = plt.subplots()
    common_csv.common_plot_cfg(fig, ax, loglin, rt, ch)

    binz = make_bins(maxe, loglin)
    for chid, eng_ary in ch.items():
        ax.hist(eng_ary, bins=binz, histtype='step', label=f'Channel {chid}')
    ax.legend(loc='upper left')

    fig.tight_layout()
    plt.show()

def main():
    try:
        channelz, run_type, maxe, ll = common_csv.common_load()
        plot_channels(channelz, run_type, maxe, ll)
    except IndexError:
        print("usage: hist_geant_csv (input file)")
        exit(1)

if __name__ == '__main__': main()
