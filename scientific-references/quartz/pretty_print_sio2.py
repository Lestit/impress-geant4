import numpy as np
import sys
sys.path.append('..')
from d2c import c_ary_print

def print_refr_idx():
    energy, real = np.loadtxt('quartz-fix.csv', unpack=True)
    to_print = {
        'QZ_REFR_IDX_ENERGIES': {'data': energy, 'units': 'eV'},
        'QZ_REFR_IDXS': {'data': real, 'units': None}
    }
    c_ary_print(to_print)

def main():
    print_refr_idx()

if __name__ == '__main__':
    with open('qz-clean.txt', 'w') as f:
        sys.stdout = f
        main()
