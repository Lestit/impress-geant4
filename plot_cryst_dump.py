import numpy as np
import matplotlib.pyplot as plt
import sys

def choose_scale():
    opts = ['log', 'linear']
    opt = ''
    while opt not in opts:
        opt = input("log or linear scale ")
    return opt


def set_scales(ax, opt):
    ax.set_xscale(opt)
    ax.set_yscale(opt)


def load_channels(fn):
    try:
        energy, cid = np.loadtxt(fn, unpack=True)
    except ValueError:
        return [], {}
    chans = {i: list() for i in range(4)}
    try:
        for (e, i) in zip(energy, cid):
            chans[i].append(e)
        return energy, chans
    except TypeError:
        return [energy], {cid: energy}


def binning_scheme(log_or_lin):
    if log_or_lin == 'log':
        return np.logspace(0, np.log10(300))
    else:
        step = 0.5
        return np.arange(0, 300 + step, step=step)


def common_g4_cfg(fig, ax, log_or_lin):
    ax.set_ylabel("Counts")
    ax.set_xlabel("Energy (keV)")
    ax.set_title("Geant dump (G4Trd)")
    ax.set_xscale(log_or_lin)
    ax.set_yscale(log_or_lin)
    ax.legend(loc='upper left')
    fig.tight_layout()


def make_hist_plots(fig, ax, energy, bins, chans, base_fn, log_or_lin, only_one=False):
    # dont plot empty
    if len(energy) == 0: return

    l = ax.get_legend()
    if l is not None: l.remove()
    ax.clear()

    for k, e in chans.items():
        ax.hist(e, bins, histtype='step', label=f"Channel {k}")
    common_g4_cfg(fig, ax, log_or_lin)
    plt.savefig(base_fn + '-full.pdf')
    if only_one:
        return

    h, edges = np.histogram(energy, bins)
    ax.set_ylim(0, np.max(h) * 0.04)
    common_g4_cfg(fig, ax, log_or_lin)
    plt.savefig(base_fn + '-cut.pdf')

    ax.clear()
    ax.hist(energy, bins, histtype='step', label='summed')
    common_g4_cfg(fig, ax, log_or_lin)
    plt.savefig(base_fn + '-sum.pdf')

    ax.set_ylim(0, np.max(h) * 0.04)
    common_g4_cfg(fig, ax, log_or_lin)
    plt.savefig(base_fn + '-sum-cut.pdf')

def main():
    if len(sys.argv) != 2:
        print("Usage: (pgy name) plot_file.csv")
        exit(1)

    log_or_lin = choose_scale()
    energy, chans = load_channels(sys.argv[1])
    bins = binning_scheme(log_or_lin)

    make_hist_plots(energy, bins, chans, sys.argv[1], log_or_lin)


if __name__ == '__main__': main()
