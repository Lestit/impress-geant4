import itertools
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import scipy.stats as st

# config!
kwds = dict(
    do_histogram=True,
    do_density=True,
    do_kde=True,
)

NUM_BINS = 2000
COLORS = itertools.cycle(list('krgbcmy') + ['orange'])

def plot_geant_counts(
        ax, data, label=None, bins=None, do_histogram=True,
        do_density=False, do_kde=False
        ):
    if do_histogram:
        ax.hist(
            data, bins=bins, label=label,
            histtype='step', color=next(COLORS), density=do_density)
    if do_kde and do_density:
        centers = bins[:-1] + np.diff(bins)
        kde = st.gaussian_kde(data, bw_method=0.009)
        ax.plot(
            centers, kde(centers), label=f'KDE {label}',
            color=next(COLORS)
        )


def load_si_cts(file_name):
    cts = []
    with open(file_name, 'r') as f:
        for line in f:
            cts.append(float(line.split()[0]))
    return np.array(cts)


def main():
    max_cts = 0
    dat = {}
    for fn in sys.argv[1:]:
        print('load', fn)
        dat[fn] = load_si_cts(fn)
        max_cts = max(max_cts, np.max(dat[fn]))
        print('done', fn)

    fig, ax = plt.subplots()
    bins = np.arange(0, max_cts, 5, dtype=int)
    for k, v in dat.items():
        plot_geant_counts(
            ax, v, label=f'{k} ({v.size} cts)',
            bins=bins,
            **kwds
        )

    ax.legend()
    ax.set_xlabel('number of counts')
    ax.set_ylabel('frequency density' if kwds['do_density'] else 'frequency')
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
