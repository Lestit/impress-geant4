import numpy as np
import sys
sys.path.append('..')
from d2c import c_ary_print

def print_refr_idx():
    energy, real, imag = np.loadtxt('al-refr-clean.csv', delimiter=',', unpack=True)
    to_print = {
        'AL_REFR_IDX_ENERGIES': {'data': energy, 'units': 'eV'},
        'AL_REFR_IDX_REAL': {'data': real, 'units': None},
        'AL_REFR_IDX_IMAG': {'data': imag, 'units': None},
    }
    c_ary_print(to_print)

def main():
    print_refr_idx()

if __name__ == '__main__':
    with open('al-clean.txt', 'w') as f:
        sys.stdout = f
        main()
