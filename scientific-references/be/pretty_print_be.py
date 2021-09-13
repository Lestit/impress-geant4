import numpy as np
import sys
sys.path.append('..')
from d2c import c_ary_print

def print_refr_idx():
    energy, real, imag = np.loadtxt('be-clean.csv', delimiter=',', unpack=True)
    to_print = {
        'BE_REFR_IDX_ENERGIES': {'data': energy, 'units': 'eV'},
        'BE_REFR_IDX_REAL': {'data': real, 'units': None},
        'BE_REFR_IDX_IMAG': {'data': imag, 'units': None},
    }
    c_ary_print(to_print)

def main():
    print_refr_idx()

if __name__ == '__main__':
    with open('be-clean-out.txt', 'w') as f:
        sys.stdout = f
        main()
