import numpy as np
import os
import sys

def cdf(ary):
    ret = np.empty(ary.size)
    for i in range(ary.size):
        ret[i] = np.sum(ary[:i[0] + 1])
    return ret

file = sys.argv[1]
e, p = np.loadtxt(file, unpack=True)
intens_cdf = cdf(p)
print(intens_cdf)

num_lookup_elts = 30000
faster_lookup = np.empty(num_lookup_elts)
cdf_idx = 0
for i, x in np.ndenumerate(np.linspace(0, 1, num_lookup_elts)):
    if x - intens_cdf[cdf_idx] < 1e-8:
        faster_lookup[i] = e[cdf_idx]
    else:
        faster_lookup[i] = e[(cdf_idx := cdf_idx + 1)]

new_fn = f"{'.'.join(sys.argv[1].split('.')[:-1])}-o1.txt"
np.savetxt(new_fn, faster_lookup, fmt="%.4f")
