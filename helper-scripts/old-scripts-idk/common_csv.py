import matplotlib.pyplot as plt
import numpy as np
import sys

def make_bins(max_eng, log_lin):
    if log_lin == 'log':
        return np.logspace(0, np.log10(max_eng * 1.2), num=1000)
    return np.linspace(1, max_eng * 1.2, num=1000)

def common_plot_cfg(fig, ax, loglin, rt, ch):
    ax.set_title(f'Geant4 output - {rt or "unspecified"}; {sum(len(v) for v in ch.values())} counts')
    ax.set_ylabel('Counts')
    ax.set_xlabel('Energy (keV)')
    ax.set_xscale(loglin)
    ax.set_yscale(loglin)

def load_channels(fn):
    channelz = {0: [], 1: [], 2: [], 3: []}
    maxe = 0
    with open(fn, 'r') as f:
        for (e, ch_id) in (l.split() for l in f):
            e = float(e)
            ch_id = int(ch_id)
            channelz[ch_id].append(e)
            maxe = max(e, maxe)

    return channelz, maxe

def common_load():
    try: run_type = sys.argv[2]
    except IndexError: run_type = ""
    # energies, channel_ids = np.loadtxt(sys.argv[1], unpack=True)
    ll = ''
    while ll not in ('log', 'linear'):
        ll = input('log or lin axes (log/linear) ')

    channelz, maxe = load_channels(sys.argv[1])
    return channelz, run_type, maxe, ll
