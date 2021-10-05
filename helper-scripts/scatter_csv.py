import numpy as np
import matplotlib.pyplot as plt
import common_csv

def hists_from_channels(maxe: float, channels: dict, log='log'):
    b = common_csv.make_bins(maxe, log)
    hists = dict()
    for k, v in channels.items():
        nh, edges = np.histogram(v, bins=b)
        hists[k] = {'hist': nh, 'edges': edges}
    return hists

def scatter_histograms(fig, ax, hg, ll, rn):
    centers = hg['edges'][:-1] + np.diff(hg['edges'])
    pts = hg['hist']
    ax.scatter(centers, pts, s=2, label='geant')
    ax.set_xlabel('energy (keV)')
    ax.set_ylabel('counts')
    cts = int(sum(pts))
    ax.set_title(f'Geant4 - scattered histogram pts - {cts} counts')
    ax.set_xscale(ll)
    ax.set_yscale(ll)

def main():
    ch, run_name, maxe, logorlin = common_csv.common_load()
    histograms = hists_from_channels(maxe, ch, logorlin)
    fig, axs = plt.subplots(2, 2)
    for hg, ax in zip(histograms.values(), axs.ravel()):
        scatter_histograms(fig, ax, hg, logorlin, run_name)
    fig.set_size_inches(12, 8)
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
