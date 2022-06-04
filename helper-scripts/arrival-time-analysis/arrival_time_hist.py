import ast
import matplotlib.pyplot as plt
import numpy as np
import os
import sys

fns = sys.argv[1:]

if len(fns) == 0:
    print("Supply silicon detection files to command line arguments to histogram the arrival times.")
    sys.exit(1)

fig, ax = plt.subplots()
NUM_BINS = 1000

for fn in fns:
    num_det = 0
    arrival_times = []
    with open(fn, 'r') as f:
        for line in f:
            stuff = line.strip().split()
            num, str_tups = stuff[0], stuff[1:]
            num_det += int(num)
            tups = [ast.literal_eval(tup) for tup in str_tups]
            arrival_times += [t[3] for t in tups]

    arrival_times = np.array(arrival_times)
    mi, ma = np.min(arrival_times), np.max(arrival_times)
    bins = np.linspace(mi, ma, num=NUM_BINS)

    lab = ' '.join(os.path.dirname(fn).split('-')[:-1])
    ax.hist(
        arrival_times, bins=bins,
        alpha=0.5, histtype='step', density=True,
        label=lab + f' {num_det} detected')

ax.set_xlabel('Photon arrival time (ns)')
ax.set_ylabel('Density')
ax.set_title(f'Arrival times for total detected opticals')
ax.legend()
# ax.set_xlim(-10, 250)
# ax.set_ylim(0, 0.042)

fig.set_size_inches(8, 6)
fig.tight_layout()
plt.show()
