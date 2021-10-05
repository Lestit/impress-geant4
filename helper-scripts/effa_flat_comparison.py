import os
import numpy as np
import sys
import matplotlib.pyplot as plt

import common_csv
import scatter_csv

srt = ['C1', 'M1', 'M5', 'X1']
pyfile_dir = os.path.dirname(__file__) + '/effa-comparison'
effa_filez = [f for f in os.listdir(pyfile_dir) if f.split('-')[0] in srt and 'effa' in f]
effa_filez.sort(key=lambda x: srt.index(x.split('-')[0]))

def plot_compare_analytical_hist(a, h, ll):
    # get htings to line up
    scale = 0.9

    fig, axs = plt.subplots(2, 2)
    axs = axs.ravel()
    for i in range(4):
        ane, anarea = a[i]
        cur_h = h[i]
        just_hist = cur_h['hist']
        just_hist = just_hist / np.max(just_hist) * np.max(anarea)
        cur_h['hist'] = just_hist
        cur_ax = axs[i]
        scatter_csv.scatter_histograms(fig, cur_ax, cur_h, ll, '')
        cur_ax.set_ylabel('scaled response (arb units)')
        cur_ax.plot(ane, scale * anarea, label='analytical', color='red')
        cur_ax.set_xscale(ll)
        cur_ax.set_yscale(ll)
        if ll == 'log': cur_ax.set_ylim(1e-4)
        cur_ax.legend()
    fig.set_size_inches(12, 8)
    fig.tight_layout()
    plt.show()


def main():
    ch, rn, maxe, logorlin = common_csv.common_load()
    histos = scatter_csv.hists_from_channels(maxe, ch, logorlin)
    analyticals = []
    for f in effa_filez:
        e, effa = np.loadtxt(os.path.join(pyfile_dir, f), unpack=True)
        analyticals.append([e, effa])

    plot_compare_analytical_hist(analyticals, histos, logorlin)

if __name__ == '__main__': main()
