import array
import itertools
import numpy as np
import struct
import sys
import matplotlib.pyplot as plt

SIZE_T_SZ = 8
DOUBLE_SZ = 8

def read_binary_si_energies(fn):
    energies = list()
    i = 0
    with open(fn, 'rb') as f:
        bin_dat = f.read()

    offset = 0
    size = len(bin_dat)
    while offset < size:
        try:
            num_pts = struct.unpack_from('N', bin_dat, offset=offset)[0]
            offset += SIZE_T_SZ
            energies.append(struct.unpack_from(num_pts * 'd', bin_dat, offset = offset))
            offset += num_pts * DOUBLE_SZ
        except IndexError:
            pass
    return energies

if __name__ == '__main__':
    # fn = 'm1-si-energies-out1638500592142.bin'
    fn = sys.argv[1]
    da_energies = read_binary_si_energies(fn)
    flat = list(itertools.chain.from_iterable(da_energies))

    wl, intens = np.loadtxt('/Users/settwi/grad_school/glesener/geant/impress/scientific-references/cebr3/cebr3-optical-burst.raw.tab', unpack=True)
    intens /= np.sum(intens)
    eng = 3e8 * 6.626e-34 / (wl * 1e-9) / 1.602e-19

    fig, ax = plt.subplots()
    bin_vals, edges = np.histogram(flat, bins=1000)
    bin_vals = np.array(bin_vals, dtype=np.float64)

    kern_sz = 10
    smoothed = np.convolve(bin_vals, np.ones(kern_sz) / kern_sz, mode='same')
    plot_pts = edges[:-1] + np.diff(edges)
    smoothed *= np.max(intens) / np.max(smoothed)

    ax.plot(eng, intens, color='green', label='original')
    ax.bar(plot_pts, smoothed, width=np.diff(edges),
           label=f'smoothed G4 (boxcar = {kern_sz})', alpha=0.5)
    ax.set_xlabel('optical photon energy (eV)')
    ax.set_ylabel('scaled density')
    ax.set_title(f'Optical photons reaching the Si detector, {len(da_energies)} interactions')
    ax.legend()
    fig.tight_layout()
    plt.show()
