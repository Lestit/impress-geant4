import itertools
import matplotlib.pyplot as plt
import numpy as np
import sys

NUM_BINS = 40
cm = plt.get_cmap('tab10')
COL_LIST = cm(np.linspace(0, 1, 10))
dup_cols = [x for c in COL_LIST for x in (c, c)]
COLORZ = itertools.cycle(dup_cols)

def slice_counts(fn):
    countz = list()
    with open(fn, 'r') as file_obj:
        for line in file_obj:
            countz.append(int(line.split()[0]))
    return np.array(countz)

def hist_cts(ctz, label):
    fig, ax = plt.gcf(), plt.gca()
    ax.hist(
        ctz, bins=NUM_BINS, label=label,
        histtype='step', alpha=0.6)

def mean_iqr_cts(ctz, label):
    # try: mean_iqr_cts._prop
    # except AttributeError: mean_iqr_cts._prop = 0.1
    fig, ax = plt.gcf(), plt.gca()
    col = next(COLORZ)
    mu = np.mean(ctz)
    q1, q2 = np.quantile(ctz, [0.25, 0.75])
    h = np.max(np.histogram(ctz, bins=NUM_BINS)[0])
    # vertical line w/ IQR poking out
    ax.vlines(mu, 0, h, colors=col, label=label + f' ~ IQR = {q2-q1:.1f}')
    ax.hlines(
        0.8 * h,
        q1, q2, colors=col, alpha=0.6)
    # mean_iqr_cts._prop += 0.05

def mean_counts_scatter(ctz, label):
    ax = plt.gca()
    h = np.max(np.histogram(ctz, bins=NUM_BINS)[0])
    ax.scatter(np.mean(ctz), h, color=next(COLORZ), label=label)
