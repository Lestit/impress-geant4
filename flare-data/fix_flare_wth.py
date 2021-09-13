import numpy as np
import os

files = [f for f in os.listdir() if 'gps' in f]
for fn in files:
    c0, c1 = np.loadtxt(fn, unpack=True)
    emin, e = c0[0], c0[1:]
    emax, intens = c1[0], c1[1:]
    with open('head_' + fn, 'w') as headf:
        np.savetxt(headf, [emin, emax])
    with open('fix_' + fn, 'w') as bulkf:
        np.savetxt(bulkf, np.transpose([e, intens]))
