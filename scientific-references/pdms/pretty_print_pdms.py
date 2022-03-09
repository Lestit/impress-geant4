import numpy as np
import scipy.constants as sc
import sys
sys.path.append('..')
from d2c import c_ary_print

def print_stuff():
    wl_um, ridx = np.loadtxt('raw-refr-idx.txt', unpack=True)
    energy = sc.h * sc.c / (wl_um * 1e-6) / sc.e
    rev = np.argsort(energy)
    want = np.linspace(0, energy.size - 1, num=20, dtype=int)
    to_print = {
        'PDMS_REFR_IDX_ENERGIES': {'data': energy[rev][want], 'units': 'eV'},
        'PDMS_REFR_IDXS': {'data': ridx[rev][want], 'units': None},
    }
    c_ary_print(to_print)


if __name__ == '__main__':
    print_stuff()
