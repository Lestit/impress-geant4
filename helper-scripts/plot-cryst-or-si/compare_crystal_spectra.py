import numpy as np
import matplotlib.pyplot as plt
import os
import sys

def die():
    print("Usage: compare_crystal_spectra incident_energy [spec1] [spec2] ...")
    print("NB: assumes only one crystal channel")
    print("NB: incident energy in keV")
    exit(1)

if len(sys.argv) < 3:
    die()

try: float(sys.argv[1])
except ValueError: die()

dens = (input('density? (y/n)').lower() == 'y')

inc_e = sys.argv[1]
files = sys.argv[2:]

low, up = 1e9, 0
energies = list()
for f in files:
    e, _ = np.loadtxt(f, unpack=True)
    energies.append(e)
    up = max(np.max(e), up)
    low = min(np.min(e), low)

fig, ax = plt.subplots()
ax.set_yscale('log')
ax.set_ylabel('Crystal energy hits')
ax.set_xlabel('Incident photon energy (keV)')

delta_e = 1
bins = np.arange(low - delta_e, up + delta_e, delta_e)
for i, e_indiv in enumerate(energies):
    lab = f'{os.path.basename(files[i]).split(".")[0]} | total counts = {e_indiv.size}'
    ax.hist(e_indiv, bins=bins, label=lab, density=dens, alpha=0.7)

ax.set_title(f'Crystal measured energy spectrum @ {inc_e} keV incident, {delta_e:.2e} keV bin width')
ax.legend()
fig.set_size_inches(8, 6)
fig.tight_layout()
plt.show()
