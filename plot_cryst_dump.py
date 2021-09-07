import numpy as np
import matplotlib.pyplot as plt
import sys

def main():
    if len(sys.argv) != 2:
        print("Usage: (pgy name) plot_file.csv")
        exit(1)

    energy, cid = np.loadtxt(sys.argv[1], unpack=True)
    chans = {i: list() for i in range(4)}
    for (e, i) in zip(energy, cid):
        chans[i].append(e)

    fig, ax = plt.subplots()
    step = 0.5
    bins = np.arange(0, np.max(energy) + step, step)

    for k, e in chans.items():
        ax.hist(e, bins, histtype='step', label=f"Channel {k}")
    ax.set_ylabel("Counts")
    ax.set_xlabel("Energy (keV)")
    ax.set_title("Geant dump (G4Trd)")
    ax.legend(loc='upper left')
    fig.tight_layout()
    plt.savefig(sys.argv[1] + '-full.pdf')

    h, edges = np.histogram(energy, bins)
    ax.set_ylim(0, np.max(h) * 0.01)
    fig.tight_layout()
    plt.savefig(sys.argv[1] + '-cut.pdf')


if __name__ == '__main__': main()
