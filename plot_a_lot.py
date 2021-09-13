import matplotlib.pyplot as plt
import numpy as np
import os
import sys

from plot_cryst_dump import make_hist_plots, load_channels

def main():
    d = sys.argv[1]
    csvs = os.listdir(d)
    bins = np.logspace(0, np.log10(300))
    fn_start = os.path.join(d, csvs[0])
    print(fn_start)
    e, c = load_channels(fn_start)
    f, a = plt.subplots()
    for fn in csvs[1:]:
        en, cn = load_channels(os.path.join(d, fn))
        e = np.append(e, en)
        for cid in c:
            c[cid] = np.append(c[cid], cn.get(cid, []))

    make_hist_plots(f, a, e, bins, c, os.path.join(d, 'sum_a_bunch'), 'log', only_one=True)

if __name__ == '__main__': main()
