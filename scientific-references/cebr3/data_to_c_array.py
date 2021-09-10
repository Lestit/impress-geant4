import numpy as np
import os
import sys
import scipy.constants as cst
import matplotlib.pyplot as plt
sys.path.append('..')
from d2c import discern_infinity, c_ary_print


def optical_emission_spectrum():
    wavelength, rel_intens = np.loadtxt('cebr3-optical-burst.raw.tab', unpack=True)
    rel_intens /= np.sum(rel_intens)
    ev = cst.value('electron volt')
    nm = 1 * cst.nano
    energy_ev = (cst.speed_of_light * cst.Planck / (wavelength * nm)) / ev

    rel_intens = np.flip(rel_intens)
    energy_ev = np.flip(energy_ev)

    to_print = {
        'CEBR3_SCINT_OPTICAL_ENERGIES': {'data': energy_ev, 'units': 'eV'},
        'CEBR3_SCINT_OPTICAL_INTENSITIES': {'data': rel_intens, 'units': None}
    }

    c_ary_print(to_print)

def refractive_index():
    energy, refr_idx = np.loadtxt('cebr3-refr-idx.tab', unpack=True)
    to_print = {
        'CEBR3_REFR_IDX_ENERGIES': {'data': energy, 'units': 'eV'},
        'CEBR3_REFR_IDXS': {'data': refr_idx, 'units': None}
    }
    c_ary_print(to_print)

def absorption_length():
    energy, inv_abs_1e5_cm = np.loadtxt('cebr3-abslength.raw.tab', unpack=True)
    # at low energies everything passes through
    energy = np.append([0], energy)
    inv_abs_1e5_cm = np.append([0], inv_abs_1e5_cm)

    abs_len = (1 / (inv_abs_1e5_cm / 1e5))
    print(energy, abs_len)
    input()
    to_print = {
        'CEBR3_ABS_LEN_ENERGIES': {'data': energy, 'units': 'eV'},
        'CEBR3_ABS_LEN': {'data': abs_len, 'units': 'cm'}
    }
    c_ary_print(to_print)

if __name__ == '__main__':
    with open('clean_data.txt', 'w') as f:
        # sys.stdout = f
        optical_emission_spectrum()
        # refractive_index()
        absorption_length()
