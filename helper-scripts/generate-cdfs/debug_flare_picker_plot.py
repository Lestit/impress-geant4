import numpy as np
import matplotlib.pyplot as plt
import sys
import os

'''
plot output from the energy_picker_test
'''

def main():
    if len(sys.argv) != 3:
        print('usage: plot_recent_keep [simulated file name] [reference flare size]')
        exit(1)

    sim_fn = sys.argv[1]
    raw_fn = os.path.join(
        os.path.dirname(os.path.realpath(__file__)),
        f'raw-flare-data/raw_{sys.argv[2].lower()}.txt')
    simmed_e = np.loadtxt(sim_fn)
    real_e, real_intens = np.loadtxt(raw_fn, unpack=True)

    fig, ax = plt.subplots()
    bins = np.logspace(0, np.log10(300), num=80)

    h, _, _ = ax.hist(simmed_e, bins=bins, alpha=0.5, label='binned output', zorder=1)
    scaled_intens = real_intens * np.max(h) / np.max(real_intens)
    ax.plot(real_e, scaled_intens, label='input (only the shape is meaningful)', zorder=2)

    num_occurs = (real_intens / np.sum(real_intens)) * simmed_e.size
    num_occurs = np.asarray(np.around(num_occurs), dtype=np.int64)
    real_occurs = np.repeat(real_e, num_occurs)
    ax.hist(real_occurs, bins=bins, label='binned (expected) input', alpha=0.6, color='red', zorder=0)

    ax.set_xlabel('energy bins (keV)')
    ax.set_ylabel('counts')
    ax.set_title(f'counts from {simmed_e.size} events compared to input')

    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlim(1, 300)

    ax.axvline(6.7, label="6.7 keV Fe", color='pink', alpha=0.5)
    ax.legend()
    fig.set_size_inches(8, 6)
    fig.tight_layout()
    plt.show()

if __name__ == '__main__': main()
