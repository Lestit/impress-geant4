import numpy as np
import scipy.constants as csts
import matplotlib.pyplot as plt
import sys
sys.path.append('..')
from d2c import c_ary_print

TEST_PLOTS = True

def efficiency():
    fn = 'sipm-efficiency-raw-wavelength.tab'
    wlen, eff = np.loadtxt(fn, unpack=True)

    # convert nanometers to eV
    energy = csts.Planck * csts.c / (wlen * 1e-9) / csts.e
    # percent to raw efficiency
    eff /= 100

    if TEST_PLOTS:
        fig, ax = plt.subplots()
        ax.plot(energy, eff)
        ax.set_xlabel('Photon energy (eV)')
        ax.set_ylabel('Photon detection efficiency')
        ax.set_title('Testing efficiency conversion')
        fig.tight_layout()
        plt.show()

    return energy, eff

def refr_idx():
    fn = 'si-n-k-wavelength.csv'
    wl, n, k = np.loadtxt(fn, unpack=True, delimiter=',')
    energy = csts.h * csts.c / (wl * 1e-6) / csts.e
    if TEST_PLOTS:
        plt.plot(energy, n, label='real idx')
        plt.plot(energy, k, label='imag idx')
        plt.legend()
        plt.gcf().tight_layout()
        plt.show()
    return energy, n, k

if __name__ == '__main__':
    eff_energy, eff = efficiency()
    refidx_energy, real_idx, imag_idx = refr_idx()
    data = {
        'SI_DET_EFF_ENERGIES': {'data': eff_energy, 'units': 'eV'},
        'SI_DET_EFF': {'data': eff, 'units': None},
        'SI_REFR_IDX_ENERGY': {'data': refidx_energy, 'units': 'eV'},
        'SI_REFR_IDX_REAL': {'data': real_idx, 'units': None},
        'SI_REFR_IDX_IMAG': {'data': imag_idx, 'units': None},
    }
    with open('silicon-data.hpp', 'w') as f:
        old_stdout = sys.stdout
        sys.stdout = f
        c_ary_print(data)
        sys.stdout = old_stdout
